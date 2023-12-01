import cadquery as cq
from typing import Literal, List
from dataclasses import dataclass, field


@dataclass
class CompartmentDoorSettings:
    """
    The settings for the compartment door
    :param compartment_door_dimensions: The dimensions of the compartment door, meaning the width, height and thickness excluding the fitting arm and tabs
    :param fitting_arm_thickness: The thickness of the fitting arm. Thicker means thicker snap joints and more tension
    :param fitting_arm_height: How deep the fitting arm goes into the case. Higher (meaning deeper) means more suspension
    :param fitting_arm_width: How wide the fitting arm is. It is not recommended to have this value be greater than the width of the compartment door
    :param tab_dimension: The dimensions of the tabs, meaning the width and height. Half of the height will be below the compartment door and the other half will stick out
                          The thickness of the tabs is the same as the thickness of the compartment door
    :param snap_joint_face_selectors: The face selectors of the faces of the compartment door where the snap joints should be placed. Uses the cadquery face selectors (eg. +Y means the face on the positive y axis)
                                      Multiple face selectors can be used by passing a list of face selectors
    :param tabs_face_selector: The face selector of the face of the compartment door where the tabs should be placed. Only one face can be selected. Uses the cadquery face selectors (eg. >Y means the face on the positive y axis)
                               Should be placed on the opposite side of the snap joints
    """
    compartment_door_dimensions: cq.Vector = (50, 60, 1.5)
    fitting_arm_thickness: float = 1.5
    fitting_arm_height: float = 10
    fitting_arm_width: float = 8
    tab_dimension: cq.Vector = (3, 3)
    snap_joint_face_selectors: List[Literal["+Y", "-Y",
                                            "+X", "-X"]] = field(default_factory=lambda: ["+Y"])
    tabs_face_selector: Literal["<Y", ">Y", "<X", ">X"] = "<Y"

    def __post_init__(self):
        self.snap_joint_face_selector = " or ".join(
            self.snap_joint_face_selectors)


@dataclass
class CompartmentDoorTolerances:
    """
    The tolerances for the compartment door
    :param snap_joint_tolerance: The tolerance for the snap joints. This is the amount of space between the snap joints and the corresponding hole in the case
    :param tab_tolerance: The tolerance for the tabs. This is the amount of extra space where the tabs will be inserted
    :param fitting_arm_tolerance: The tolerance for the fitting arm. This is the amount of extra space (height and width) where the fitting arm will be inserted
    :param recessed_edge_width: The width of the recessed edge. This is the amount of extra space on which the compartment door will be placed to not have it stick out and not have it fall in
    :param recess_edge_depth: TODO: add description
    :param compartment_door_tolerance: The tolerance for the compartment door. This is the amount of extra space where the compartment door will be inserted
    """
    snap_joint_tolerance: float = 0.5
    tab_tolerance: float = 0.5
    fitting_arm_tolerance: float = 0.5
    recessed_edge_width: float = 2
    recess_edge_depth: float = 5
    compartment_door_tolerance: float = 0.5


def generate_compartment_door(settings: CompartmentDoorSettings = CompartmentDoorSettings()) -> cq.Workplane:
    """
    Generates a compartment door with snap joints and tabs
    :param settings: The settings for the compartment door (see CompartmentDoorSettings for more information)
    :return: The compartment door as a cadquery Workplane

    Example:

    .. code-block:: python

            from ocp_vscode import show_object
            from compartment_door import generate_compartment_door, CompartmentDoorSettings

            show_object(generate_compartment_door(CompartmentDoorSettings()))
    """
    compartment_door_dimensions = settings.compartment_door_dimensions
    fitting_arm_thickness = settings.fitting_arm_thickness
    fitting_arm_height = settings.fitting_arm_height
    fitting_arm_width = settings.fitting_arm_width
    tab_dimension = settings.tab_dimension
    snap_joint_face_selectors = settings.snap_joint_face_selectors
    tabs_face_selector = settings.tabs_face_selector

    snap_joint_face_selector = " or ".join(snap_joint_face_selectors)

    return (cq.Workplane("XY")
            # compartment_door
            .box(compartment_door_dimensions[0], compartment_door_dimensions[1], compartment_door_dimensions[2])
            .faces(snap_joint_face_selector)
            .each(
        lambda cq_object: cq.Workplane(cq_object)
        # fitting_arm
        .workplane()
        .transformed(offset=(0, 0.5 * compartment_door_dimensions[2], 0), rotate=(0, -90, 0))
        .line(fitting_arm_thickness, 0)
        .line(0.5, -fitting_arm_height + 1.5 * fitting_arm_thickness)
        .threePointArc((2 * fitting_arm_thickness + 0.5, -fitting_arm_height + fitting_arm_thickness), (3 * fitting_arm_thickness, -fitting_arm_height + 1.5 * fitting_arm_thickness))
        .line(0.5, +fitting_arm_height - 1.5 * fitting_arm_thickness)
        .line(fitting_arm_thickness, 0)
        .line(-0.5, -fitting_arm_height + 1.5 * fitting_arm_thickness)
        .threePointArc((2 * fitting_arm_thickness + 0.5, -fitting_arm_height), (0.5, -fitting_arm_height + 1.5 * fitting_arm_thickness))
        .lineTo(0, -compartment_door_dimensions[2])
        .lineTo(0, 0)
        .close()
        .extrude(fitting_arm_width, both=True)
        # fitting_arm_hole
        .transformed(rotate=(90, 0, 0))
        .moveTo(3.5 * fitting_arm_thickness + 1, 0)
        .rect(fitting_arm_width/1.5, fitting_arm_width)
        .cutBlind(2*fitting_arm_thickness)
        # fitting_arm_snap_joint bottom
        .tag("a")
        .faces("<Z[-1]")
        .transformed(offset=(0, 0, 2*fitting_arm_thickness))
        .moveTo(3.5 * fitting_arm_thickness + 1, 0)
        .rect(fitting_arm_thickness, fitting_arm_width, centered=(False, True))
        .extrude(fitting_arm_thickness)
        .workplaneFromTagged("a")
        # fitting_arm_snap_joint top
        .center(3 * fitting_arm_thickness + 0.5, 0)
        .move(0, -fitting_arm_width/2)
        .line(0, -fitting_arm_thickness)
        .line(1.5*fitting_arm_thickness, 0)
        .threePointArc((2.5*fitting_arm_thickness, 0), (1.5*fitting_arm_thickness, fitting_arm_width/2 + fitting_arm_thickness))
        .line(-1.5*fitting_arm_thickness, 0)
        .line(0, -fitting_arm_thickness)
        .line(fitting_arm_thickness, 0)
        .threePointArc((1.5 * fitting_arm_thickness, 0), (fitting_arm_thickness, -fitting_arm_width/2))
        .line(-fitting_arm_thickness, 0)
        .close()
        .extrude(fitting_arm_thickness)
        .val(),
        combine="a"
    )
        # tabs
        .faces(tabs_face_selector)
        .workplane()
        .moveTo(-compartment_door_dimensions[0] / 4, -compartment_door_dimensions[2])
        .line(compartment_door_dimensions[0] / 2, 0, forConstruction=True)
        .vertices()
        .rect(tab_dimension[0], fitting_arm_thickness)
        .extrude(tab_dimension[1], both=True)
    )


def generate_compartment_door_cutout(settings: CompartmentDoorSettings = CompartmentDoorSettings(), tolerances: CompartmentDoorTolerances = CompartmentDoorTolerances()) -> cq.Workplane:
    """
    Generates a compartment door coutout which can be subtracted from the case to make space for the compartment door to snap in
    :param settings: The settings for the compartment door that is supposed to snap in (see CompartmentDoorSettings for more information)
    :param tolerances: The tolerances for the compartment door (see CompartmentDoorTolerances for more information)

    Example:

    .. code-block:: python

            from ocp_vscode import show_object
            from compartment_door import generate_compartment_door, CompartmentDoorSettings, CompartmentDoorTolerances

            show_object(generate_compartment_door(CompartmentDoorSettings()), name="compartment_door")
            show_object(generate_compartment_door_cutout(CompartmentDoorSettings(), CompartmentDoorTolerances()), name="compartment_door_cutout")
    """
    s = settings
    t = tolerances

    return (cq.Workplane("XY")
            # compartment_door and recessed edge
            .box(s.compartment_door_dimensions[0] + 2 * t.compartment_door_tolerance, s.compartment_door_dimensions[1] + 2 * t.compartment_door_tolerance, s.compartment_door_dimensions[2])
            .tag("box")
            .faces("<Z")
            .workplane()
            .rect(s.compartment_door_dimensions[0] - 2 * t.recessed_edge_width, s.compartment_door_dimensions[1] - 2 * t.recessed_edge_width)
            .extrude(t.recess_edge_depth)
            # fitting_arms
            .faces(s.snap_joint_face_selector, tag="box")
            .each(
        lambda cq_object: cq.Workplane(cq_object)
        .workplane(offset=-t.compartment_door_tolerance - t.fitting_arm_tolerance)
        .transformed(offset=(0, 0.5 * s.compartment_door_dimensions[2], 0), rotate=(0, -90, 0))
        .line(4*s.fitting_arm_thickness + 0.5 + 2 * t.fitting_arm_tolerance, 0)
        .line(0, - 2 * s.fitting_arm_thickness)
        .line(s.fitting_arm_thickness, 0)
        .line(0, -s.fitting_arm_thickness - t.fitting_arm_tolerance)
        .line(-s.fitting_arm_thickness, 0)
        .line(-0.5, 3 * s.fitting_arm_thickness - s.fitting_arm_height)
        .line(-4*s.fitting_arm_thickness + 0.5 - 2 * t.fitting_arm_tolerance, 0)
        .line(-0.5, s.fitting_arm_height + t.fitting_arm_tolerance)
        .close()
        .extrude(s.fitting_arm_width + t.fitting_arm_tolerance, both=True)
        # fitting_arm_snap_joint
        .faces("-Z")
        .transformed(rotate=(90, 0, -90))
        .center(0, 4 * s.fitting_arm_thickness + 0.5)
        .rect(2 * s.fitting_arm_width + 2 * t.fitting_arm_tolerance, 2 * s.fitting_arm_thickness + t.fitting_arm_tolerance, centered=(True, False))
        .extrude(s.fitting_arm_thickness)
        .val(),
        combine="a"
    )
        # tabs
        .faces(s.tabs_face_selector)
        .workplane(offset=-t.compartment_door_tolerance - t.tab_tolerance)
        .moveTo(-s.compartment_door_dimensions[0] / 4, -0.5*s.compartment_door_dimensions[2])
        .line(s.compartment_door_dimensions[0] / 2, 0, forConstruction=True)
        .vertices()
        .rect(s.tab_dimension[0] + 2 * t.tab_tolerance, s.fitting_arm_thickness + 2 * t.tab_tolerance)
        .extrude(s.tab_dimension[1] + 2 * t.tab_tolerance, both=True)
    )


if __name__ == "__main__":
    from ocp_vscode import show_object
    compartmentDoorSettings = CompartmentDoorSettings(
        snap_joint_face_selectors=["+X", "-X"], tabs_face_selector="<Y")
    compartmentDoorTolerances = CompartmentDoorTolerances()
    show_object(generate_compartment_door(
        compartmentDoorSettings), name="compartment_door")
    show_object(generate_compartment_door_cutout(
        compartmentDoorSettings, compartmentDoorTolerances), name="compartment_door_cutout")