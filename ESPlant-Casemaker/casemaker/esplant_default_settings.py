from settings import *
from components import battery_springs


board_settings = BoardSettings(part_settings=[
    PartSetting(".*", ">Z", 0.5),
    PartSetting(".*", "<Z", 0.5),
    PartSetting(".*USB.*", "<X",
                "Hole", width=11, height=6.5),
    PartSetting(".*SW-SMD_L3.9-W3.0-P4.45.*", ">Z", "Hole"),
    PartSetting(".*SW-SMD_MK-12C02-G025.*", ">X", "Hole", height=4),
    PartSetting(".*LED.*", ">Z", "Hole"),
    PartSetting(".*ALS-PT19.*", ">Z", "Hole"),
    PartSetting(".*ESP32.*", "<Z", 2),
    PartSetting(".*ESP32.*", ">Z", 2),
],
    parts_without_tolerances=["BatterySprings"],
    exclude=["PinHeader"]
)
case_settings = CaseSettings(
    case_dimension=("Auto", 55, 11),
    case_offset=(0, "Positive", "Positive")
)
compartment_door_settings = CompartmentDoorSettings(tab_spacing_factor=0.8, fitting_arm_height=11)
battery_holder_settings = BatteryHolderSettings(
    front_wall_thickness=2.5,
    back_wall_thickness=2,
    insertable_springs_thickness=1,
    polartiy_text_spacing=0.2,
    battery_length_tolerance=2,
    battery_diameter_tolerance=0.25,
    offset=(0, 0.2, 0),
    flip_polarity=True
)

mounting_hole_diameter = 3.2

default_mounting_hole_settings = MountingHoleSettings(
    tolerance=0.2,
)

additional_parts = {
    "BatterySprings": battery_springs.val().wrapped,
}
