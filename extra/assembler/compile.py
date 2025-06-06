from dataclasses import dataclass
from enum import StrEnum
from typing import Tuple

from parser import Line, Op
from bin import op_to_bytes


class DiagnosticKind(StrEnum):
    Warn = "Warning"
    Err = "Error"
    Info = "Info"


class Compiler:
    def __init__(self, **kwargs):
        self.bytes = bytearray(0x200)
        self.diagnostics = []

        self.name_resolution_table = {}
        self.label_offsets = {}

        self.default_label_location = 0x200

    def concat_instruction(self, *instructions):
        for instruction in instructions:
            bs = op_to_bytes(instruction)
            if bs is None:
                self.warn(f"No instruction {instruction} found, ignoring")
                continue
            self.bytes.extend(bs)

    def try_concat_name(self, name, args):
        if name not in self.name_resolution_table:
            self.warn(f"Could not find name `{name}`, inserting `_break`")
            self.concat_instruction((Op.Break,))
            return

    @property
    def offset(self) -> int:
        return len(self.bytes)

    def insert_label(self, label, args = {}):
        self.label_offsets[label] = self.offset

    def finalize(self) -> bytearray:
        return self.bytes.copy()

    def warn(self, s):
        self.diagnostics.append((DiagnosticKind.Warn, s))


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


def compile(lines: list[Line], **kwargs):
    c = Compiler()

    for line in lines:
        match line:
            case (Line.Instruction, *instruction):
                c.concat_instruction(instruction)
            case (Line.Name, name, *args):
                c.try_concat_name(name, args)
            case (Line.Label, label):
                c.insert_label(label)
    
    for _, line in c.diagnostics:
        print(line)
    
    return c.finalize()
