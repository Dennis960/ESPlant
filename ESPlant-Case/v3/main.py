try:
    from ocp_vscode import show_object
except ImportError:
    show_object = lambda *any: None
from utils import load_parts, extrude_part_faces, extrude_part_width, extrude_part_height
from components import battery_springs
import cadquery as cq
from board_converter import convert_if_needed
import re

# Converts the pcb board and generates the parts.json file if it doesn't exist yet
convert_if_needed()

###----------------- Settings -----------------###
filename = "ESPlant-Case/v3/ESPlant-Case.step"
# minimum_width = 0.5
minimum_wall_thickness = 1.5
# closure_tolerance = 0.5
# hole_tolerance = 0.1
# board_tolerance_xy = 1.5
board_tolerance_z = 0.5
part_tolerance = 1

case_hole_extrusion_size = 50

HOLE = None
parts_to_exclude = ["PinHeader"]
parts_to_keep_original_shape = ["PCB"]
parts_to_extrude_for_case_bottom = [
    {"name": "", "direction": ">Z", "length": board_tolerance_z},
    {"name": "", "direction": "<Z", "length": board_tolerance_z},
    {"name": "MICRO-USB", "direction": ">X", "length": HOLE, "min_width": 11, "min_height": 6.5},
    {"name": "SW-SMD_4P", "direction": ">Z", "length": HOLE},
    {"name": "SW-SMD_MK", "direction": ">Z", "length": HOLE, "offset_y": -2, "min_height": 10},
    {"name": "LED", "direction": ">Z", "length": HOLE},
    {"name": "ALS-PT19","direction": ">Z", "length": HOLE},
    {"name": "PCB","direction": "<Z", "length": HOLE},
    {"name": "ESP","direction": "<Z", "length": HOLE},
    {"name": "ESP","direction": ">Z", "length": 2},
]

case_height = 62

###----------------- Board + Components (Original) -----------------###
board = cq.importers.importStep("ESPlant-Case/v3/ESPlant-Board.step")
board = board.union(battery_springs)


###----------------- Board + Components (Boxes) -----------------###
parts_raw, parts_boxes, parts_names = load_parts(parts_exclude=parts_to_exclude)

def part_indices_of(name_re: str):
    """
    Get the indices of all parts that include the given regular expression.
    """
    return [i for i, part_name in enumerate(parts_names) if re.match(f".*{name_re}.*", part_name)]

for i in range(len(parts_raw)):
    if any(part_keep_shape in parts_names[i] for part_keep_shape in parts_to_keep_original_shape):
        parts_boxes[i] = extrude_part_faces("<Z", parts_raw[i], board_tolerance_z)
    else:
        parts_boxes[i] = parts_boxes[i].union(parts_boxes[i].faces("<Z").shell(part_tolerance, kind="intersection"))

parts_hole_extrusions = []

for part_to_extrude in parts_to_extrude_for_case_bottom:
    part_name = part_to_extrude["name"]
    extrude_dir = part_to_extrude["direction"]
    is_hole_extrusion = part_to_extrude["length"] is HOLE
    extrude_len = case_hole_extrusion_size if is_hole_extrusion else part_to_extrude["length"]
    part_indices = part_indices_of(part_name)
    for part_index in part_indices:
        extrusion = extrude_part_faces(extrude_dir, parts_boxes[part_index], extrude_len)
        if "min_width" in part_to_extrude:
            extrusion = extrude_part_width(extrusion, part_to_extrude["min_width"], extrude_dir)
        if "min_height" in part_to_extrude:
            extrusion = extrude_part_height(extrusion, part_to_extrude["min_height"], extrude_dir)
        if "offset_x" in part_to_extrude:
            extrusion = extrusion.translate((part_to_extrude["offset_x"], 0, 0))
        if "offset_y" in part_to_extrude:
            extrusion = extrusion.translate((0, part_to_extrude["offset_y"], 0))
        if "offset_z" in part_to_extrude:
            extrusion = extrusion.translate((0, 0, part_to_extrude["offset_z"]))
        if is_hole_extrusion:
            parts_hole_extrusions.append(extrusion)
        else:
            parts_boxes[part_index] = parts_boxes[part_index].union(extrusion)

# combine all parts into one object
part_union = cq.Workplane("XY")
for part in parts_boxes:
    part_union = part_union.union(part)
part_union = part_union.union(battery_springs)

###----------------- Case -----------------###
# get bounding box of all parts
part_union_bounding_box = part_union.val().BoundingBox()
part_union_center = part_union.val().CenterOfBoundBox()
part_union_box = cq.Workplane("XY").box(part_union_bounding_box.xlen, case_height, part_union_bounding_box.zlen).translate((part_union_center.x, part_union_center.y, part_union_center.z))
part_union_box = part_union_box.translate((0, part_union_bounding_box.ylen / 2 - case_height/2, 0))
part_union_shell = part_union_box.faces("<Z").shell(minimum_wall_thickness, kind="intersection")

part_union_shell = part_union_shell.union(extrude_part_faces("<Z", part_union_shell, 8, faces_selector=">Z[-2]"))

# cut out holes and parts
part_union_shell = part_union_shell.cut(part_union)
for part_hole_extrusion in parts_hole_extrusions:
    part_union_shell = part_union_shell.cut(part_hole_extrusion)

# TODO use holes to fixate the case

###----------------- Preview -----------------###
# for part_name, bounding_box_part in zip(part_names, part_bounding_boxes):
#     show_object(bounding_box_part, name=part_name)
# show_object(battery_springs, name="battery_springs")
show_object(board, name="board")
# show_object(parts, name="parts")
# show_object(part_union, name="part_union")
show_object(part_union_shell, name="case_bottom")

###----------------- Export -----------------###
cq.Assembly(part_union_shell).save(filename)
