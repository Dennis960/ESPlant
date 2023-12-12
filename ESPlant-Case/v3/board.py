from OCP.TopoDS import TopoDS_Shape
from pcb import make_offset_shape
from functools import cache
from typing import List
import cadquery as cq
import re
from utils import extrude_part_faces, extrude_part_width, extrude_part_height
from settings import BoardSettings, HOLE_TYPE, DIMENSION_TYPE, PCB_PART_NAME, case_hole_extrusion_size, PartSetting



class Board:
    def __init__(self, shapes_dict: dict[str, TopoDS_Shape], board_name: str, board_settings: BoardSettings = BoardSettings()):
        self._board_name = board_name
        self._board_cq_object, self._shapes_dict = self._remove_board_from_shapes_dict(
            shapes_dict | board_settings.additional_parts_dict)
        self._cq_object_dict = {name: cq.Workplane(cq.Shape.cast(
            shape)) for name, shape in self._shapes_dict.items()}
        self._bounding_box_dict = {name: cq.Shape.cast(
            shape).BoundingBox() for name, shape in self._shapes_dict.items()}
        self._bounding_box_cq_object_dict = self._convert_bounding_box_dict_to_bounding_box_cq_object_dict(
            self._bounding_box_dict)
        self._board_settings = board_settings
        self._board_settings.part_settings = self._move_hole_settings_to_end(
            self._board_settings.part_settings)
        self._pcb_cq_object_with_tolerance = self.get_pcb_cq_object_with_tolerance()
        self._cq_object_with_part_tolerance_dict = self._get_cq_object_with_part_tolerance_dict()
        self._cq_object_with_part_tolerance_and_applied_settings_dict, self._hole_dict = self._apply_settings_to_cq_object_with_part_tolerance_dict()

    @cache
    def get_cq_objects_with_tolerances_union(self):
        """
        Returns a cq object that can be cut out of the case to make space for the board and components with their tolerances
        """
        cq_objects_with_tolerances_union = cq.Workplane("XY")
        for cq_object_with_part_tolerance in self._cq_object_with_part_tolerance_and_applied_settings_dict.values():
            cq_objects_with_tolerances_union = cq_objects_with_tolerances_union.union(
                cq_object_with_part_tolerance)
        return cq_objects_with_tolerances_union

    @cache
    def get_holes_union(self):
        """
        Returns a cq object that can be cut out of the case to create the holes
        """
        holes_union = cq.Workplane("XY")
        for hole_cq_object in self._hole_dict.values():
            holes_union = holes_union.union(hole_cq_object)
        return holes_union

    def _find_all_cq_objects_by_name_regex(self, regex: str):
        return [cq_object for name, cq_object in self._cq_object_dict.items() if re.match(f".*{regex}.*", name)]

    def _find_all_names_by_name_regex(self, regex: str):
        return [name for name in self._shapes_dict.keys() if re.match(f".*{regex}.*", name)]

    def _move_hole_settings_to_end(self, part_settings: List[PartSetting]):
        offset = 0
        for i in range(len(part_settings)):
            if part_settings[i + offset].length is HOLE_TYPE.HOLE:
                part_settings.append(part_settings.pop(i + offset))
                offset -= 1
        return part_settings

    def _remove_board_from_shapes_dict(self, shapes_dict: dict[str, TopoDS_Shape]) -> tuple[cq.Workplane, dict[str, TopoDS_Shape]]:
        board_cq_object: cq.Workplane
        for name, shape in shapes_dict.items():
            if self._board_name in name:
                board_cq_object = cq.Workplane(cq.Shape.cast(shape))
                del shapes_dict[name]
                break
        return board_cq_object, shapes_dict

    def _get_cq_object_with_part_tolerance_dict(self) -> dict[str, cq.Workplane]:
        s = self._board_settings
        cq_object_with_tolerance_dict = {}
        for name in self._shapes_dict.keys():
            if PCB_PART_NAME in name:
                cq_object_with_tolerance = self._pcb_cq_object_with_tolerance
            else:
                bounding_box_cq_object = self._bounding_box_cq_object_dict[name]
                cq_object_with_tolerance = bounding_box_cq_object.union(
                    bounding_box_cq_object.faces("<Z").shell(
                        s.part_tolerance, kind="intersection"
                    )
                )
            cq_object_with_tolerance_dict[name] = cq_object_with_tolerance
        return cq_object_with_tolerance_dict

    @cache
    def get_pcb_cq_object_with_tolerance(self):
        """
        Returns the pcb cq object with the tolerance applied as well as the fixation holes
        """
        s = self._board_settings
        pcb_cq_object = self._cq_object_dict[self._find_all_names_by_name_regex(PCB_PART_NAME)[
            0]]
        return make_offset_shape(
            pcb_cq_object,
            s.pcb_tolerance, s.should_use_fixation_holes, s.fixation_hole_diameter, s.fixation_hole_tolerance,
            s.fixation_hole_bigger_diameter, s.pcb_thickness, s.pcb_tolerance.x
        )

    def _apply_settings_to_cq_object_with_part_tolerance_dict(self) -> tuple[dict[str, cq.Workplane], dict[str, cq.Workplane]]:
        cq_object_with_part_tolerance_dict = self._cq_object_with_part_tolerance_dict.copy()
        hole_cq_objects_dict: dict[str, cq.Workplane] = {}
        for part_setting in self._board_settings.part_settings:
            names = self._find_all_names_by_name_regex(part_setting.name_regex)
            for name in names:
                cq_object_with_applied_setting = cq_object_with_part_tolerance_dict[name]
                if part_setting.length is not HOLE_TYPE.HOLE:
                    cq_object_with_applied_setting = self._apply_setting_to_cq_object(
                        cq_object_with_applied_setting, part_setting)
                    cq_object_with_part_tolerance_dict[name] = cq_object_with_applied_setting
                else:
                    if name not in hole_cq_objects_dict:
                        hole_cq_objects_dict[name] = cq.Workplane("XY")
                    hole_cq_object = hole_cq_objects_dict[name]
                    hole_cq_object = hole_cq_object.union(
                        self._apply_setting_to_cq_object(
                            cq_object_with_applied_setting, part_setting)
                    )
                    hole_cq_objects_dict[name] = hole_cq_object
        return cq_object_with_part_tolerance_dict, hole_cq_objects_dict

    def _apply_setting_to_cq_object(self, cq_object: cq.Workplane, part_setting: PartSetting):
        is_hole_extrusion = part_setting.length is HOLE_TYPE.HOLE
        extrude_len = (
            case_hole_extrusion_size if is_hole_extrusion else part_setting.length
        )
        extrusion = extrude_part_faces(
            cq_object, part_setting.top_direction, extrude_len
        )
        if part_setting.width is not DIMENSION_TYPE.AUTO:
            extrusion = extrude_part_width(
                extrusion, part_setting.width, part_setting.top_direction
            )
        if part_setting.height is not DIMENSION_TYPE.AUTO:
            extrusion = extrude_part_height(
                extrusion, part_setting.height, part_setting.top_direction
            )
        extrusion = extrusion.translate(
            (part_setting.offset_x, part_setting.offset_y, part_setting.offset_z)
        )
        if is_hole_extrusion:
            extrusion = cq_object.union(extrusion)
        else:
            extrusion = cq_object.union(extrusion)
        return extrusion

    def _convert_bounding_box_dict_to_bounding_box_cq_object_dict(self, bounding_box_dict: dict[str, cq.BoundBox]) -> dict[str, cq.Workplane]:
        return {name:
                cq.Workplane("XY").box(
                    bounding_box.xlen,
                    bounding_box.ylen,
                    bounding_box.zlen
                ).translate((
                    bounding_box.center.x,
                    bounding_box.center.y,
                    bounding_box.center.z
                ))
                for name, bounding_box in bounding_box_dict.items()
                }