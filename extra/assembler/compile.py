from typing import Tuple

from parser import Line, Instruction, Op
from bin import op_to_bytes


def group_labels(lines: list[Line]) -> (dict[str, list], list[Tuple[str, list[Line]]]):
    label_args = {}
    labelled_groups = []

    active_line = None
    ongoing_group = []
    for line in lines:
        match line:
            case (Line.Label, label):
                if active_line or ongoing_group:
                    label_args[label[0]] = label[1:]
                    labelled_groups.append((active_line, ongoing_group))
                active_line = label
                ongoing_group = []
            case _:
                ongoing_group.append(line)

    if ongoing_group:
        labelled_groups.append((active_line, ongoing_group))

    return (label_args, labelled_groups)


def compile(lines: list[Line]):
    label_args, label_groups = group_labels(lines)

    out = bytearray(0x200)

    label_locations = {}
    labels_to_resolve_later = {}
    offset = 0x200
    for label, group in label_groups:
        label_locations[label] = offset
