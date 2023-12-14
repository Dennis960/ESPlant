from OCP.TopoDS import TopoDS_Shape
from cadquery import Vector
from board_converter import BoardConverter
from case import Case
from compartment_door import CompartmentDoor
from battery_holder import BatteryHolderSettings, BatteryHolder
from board import Board
from settings import CaseSettings, BoardSettings, CompartmentDoorSettings, ALIGNMENT, DIMENSION_TYPE, SIDE
from components import battery_springs
from utils import get_rotation_for_side

from serializer import register
register()


class CasemakerLoader:
    """
    Helper class to load different file formats and create a Casemaker object from it.
    """

    def __init__(self, cache_directory: str = "parts"):
        self.cache_dir = cache_directory

    def load_kicad_pcb(self, kicad_pcb_path: str, step_file: str = "board.step"):
        """
        Loads the kicad_pcb file and creates a Casemaker object from it.

        :param kicad_pcb_path: Absolute path to the kicad_pcb file
        :param step_file: Name of the file in the cache directory
        """
        board_shape, shapes_dict = (BoardConverter(self.cache_dir)
                                    .from_kicad_pcb(kicad_pcb_path, step_file)
                                    )
        return Casemaker(board_shape, shapes_dict, self.cache_dir)

    def load_step_file(self, step_file: str = "board.step"):
        """
        Loads the step file and creates a Casemaker object from it.

        :param step_file: Name of the file in the cache directory
        """
        board_shape, shapes_dict = (BoardConverter(self.cache_dir)
                                    .from_step_file(step_file)
                                    )
        return Casemaker(board_shape, shapes_dict, self.cache_dir)

    def load_pickle(self, pickle_file: str = "board.pickle"):
        """
        Loads the pickle file and creates a Casemaker object from it.

        :param pickle_file: Name of the pickle file
        """
        board_shape, shapes_dict = (BoardConverter(self.cache_dir)
                                    .from_pickle(pickle_file)
                                    )
        return Casemaker(board_shape, shapes_dict, self.cache_dir)


class Casemaker:
    """
    Do not use this class directly if you don't know what you are doing.\n
    Use the CasemakerLoader class instead.
    """

    def __init__(self, board_shape: TopoDS_Shape, shapes_dict: dict[str, TopoDS_Shape], cache_dir: str = "parts"):
        self.board_shape = board_shape
        self.shapes_dict = shapes_dict
        self.cache_dir = cache_dir

    def generate_board(self, board_settings: BoardSettings = BoardSettings(), exclude: list[str] = [], additional_parts: dict[str, TopoDS_Shape] = {}):
        """
        Generates a board object from the board shape and the shapes dictionary with the specified settings.

        :param board_settings: The settings for the board object.
        :param exclude: List of part names to exclude. Does not need to be the full name, only a part of it.
        (e.g. "PinHeader" will exclude all parts that contain "PinHeader" in their name, such as "PinHeader_1x2")
        :param additional_parts: A dictionary of names and TopoDS_Shape objects to add to the shapes dictionary.
        """
        shapes_dict = {**self.shapes_dict, **additional_parts}
        shapes_dict = {name: shape for name,
                       shape in shapes_dict.items() if not any(ex in name for ex in exclude)}
        board = Board(self.board_shape, shapes_dict, board_settings)
        return CasemakerWithBoard(self.board_shape, self.shapes_dict, board)

    def save_pickle(self, pickle_file: str = "board.pickle"):
        """
        Saves the board_shape and shapes_dict to a pickle file.

        :param pickle_file: Name of the pickle file
        """
        BoardConverter(self.cache_dir).save_to_pickle(
            self.board_shape, self.shapes_dict, pickle_file)
        return self

    def save_step_file(self, step_file: str = "board-saved.step"):
        """
        Saves the board with its components as a step file.

        :param step_file: Name of the step file
        """
        BoardConverter(self.cache_dir).save_assembly(
            self.shapes_dict, step_file, exportType="STEP")
        return self

    def save_gltf_file(self, gltf_file: str = "board.gltf", tolerance: float = 0.1, angularTolerance: float = 0.1):
        """
        Saves the board with its components as a gltf file.

        :param gltf_file: Name of the gltf file
        """
        BoardConverter(self.cache_dir).save_assembly(
            self.shapes_dict, gltf_file, exportType="GLTF", tolerance=tolerance, angularTolerance=angularTolerance)
        return self


class CasemakerWithBoard:
    """
    Do not use this class directly if you don't know what you are doing.\n
    Use the CasemakerLoader class instead.
    """

    def __init__(self, board_shape: TopoDS_Shape, shapes_dict: dict[str, TopoDS_Shape], board: Board):
        self.board_shape = board_shape
        self.shapes_dict = shapes_dict
        self.board = board

    def generate_case(self, case_settings: CaseSettings = CaseSettings()):
        """
        Generates a case object using information from the board object.
        """
        case = Case(self.board, case_settings)
        return CasemakerWithCase(self.board_shape, self.shapes_dict, self.board, case)


class CasemakerWithCase:
    """
    Do not use this class directly if you don't know what you are doing.\n
    Use the CasemakerLoader class instead.
    """

    def __init__(self, board_shape: TopoDS_Shape, shapes_dict: dict[str, TopoDS_Shape], board: Board, case: Case):
        self.board_shape = board_shape
        self.shapes_dict = shapes_dict
        self.board = board
        self.case = case
        self.compartment_door: CompartmentDoor = None
        self.battery_holder: BatteryHolder = None

    def add_compartment_door(self, side: SIDE, compartment_door_settings: CompartmentDoorSettings = CompartmentDoorSettings()):
        """
        Adds a compartment door to the case at the specified side.
        Currently only one compartment door can be added per case.
        """
        face_width, face_height = self.case.get_dimension_of_side(side)

        # set the dimensions of the compartment door to match the dimensions of the face where it is placed
        compartment_door_settings.compartment_door_dimensions = Vector(
            face_width - 2 * self.case.settings.case_wall_thickness, face_height - 2 * self.case.settings.case_wall_thickness, 1.5)

        self.compartment_door = CompartmentDoor(compartment_door_settings)
        self.compartment_door.translate(
            (0, 0, -0.5 * self.case.settings.case_wall_thickness))

        # rotate the compartment door to match the side
        self.compartment_door.rotate(*get_rotation_for_side(side))

        # move the compartment door to the correct position
        self.compartment_door.translate(self.case.get_center_of_side(side))

        # unite the compartment door frame with the case
        self.case.case_cq_object = self.case.case_cq_object.union(self.compartment_door.frame).cut(
            self.compartment_door.door_with_tolerance).cut(self.case.get_cuts())
        self.compartment_door.door = self.compartment_door.door.cut(
            self.board.get_pcb_cq_object_with_tolerance())
        return self

    def add_battery_holder(self, side: SIDE, battery_holder_settings: BatteryHolderSettings = None):
        """
        Adds a battery holder to the case at the specified side.
        Currently only one battery holder can be added per case.
        """
        self.battery_holder = BatteryHolder(battery_holder_settings)

        # rotate the battery holder to match the side
        self.battery_holder.rotate(*get_rotation_for_side(side))

        self.battery_holder.translate(self.case.get_center_of_side(side))

        self.battery_holder.battery_holder = self.battery_holder.battery_holder.cut(
            self.board.get_pcb_cq_object_with_tolerance())
        return self


if __name__ == "__main__":
    from ocp_vscode import show_all
    import logging
    import os

    logging.basicConfig(level=logging.INFO)

    if not os.path.exists("parts/board.pickle"):
        CasemakerLoader().load_kicad_pcb(
            "ESPlant-Board/ESPlant-Board.kicad_pcb").save_pickle()

    casemaker = (CasemakerLoader()
                 .load_pickle("board.pickle")
                 # .load_kicad_pcb("ESPlant-Board/ESPlant-Board.kicad_pcb")
                 # .load_step_file("board.step")
                 # .save_step_file("board-saved.step")
                 # .save_pickle("board.pickle")
                 .save_gltf_file("board.gltf")
                 .generate_board(BoardSettings(), exclude=["PinHeader"], additional_parts={
                     "BatterySprings": battery_springs.val().wrapped,
                 })
                 .generate_case(CaseSettings(
                     case_dimension=(DIMENSION_TYPE.AUTO, 62, 12),
                     case_offset=(0, ALIGNMENT.POSITIVE, ALIGNMENT.POSITIVE)
                 ))
                 .add_compartment_door(SIDE.BOTTOM, CompartmentDoorSettings(
                     tab_spacing_factor=0.8,
                 ))
                 .add_battery_holder(SIDE.BOTTOM, BatteryHolderSettings(
                     front_wall_thickness=2.5,
                     back_wall_thickness=1.5,
                     insertable_springs_thickness=1,
                     polartiy_text_spacing=0.3,
                     battery_length_tolerance=4
                 ))
                 )

    show_all({
        "board": casemaker.board.board_cq_object,
        "case_bottom": casemaker.case.case_cq_object,
        "compartment_door": casemaker.compartment_door.door,
        "battery_holder": casemaker.battery_holder.battery_holder,
        # "batteries": casemaker.battery_holder.batteries,
    })