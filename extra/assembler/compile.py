from dataclasses import dataclass
from enum import StrEnum
from typing import Tuple

from parser import Line, Op, parse
from bin import op_to_bytes
from utils import hi_lo


def name_jump(c: 'Compiler'):
    hi, lo = hi_lo(c.offset)
    c.concat_inline("_push rA", "_const")
    c.concat_raw(hi)
    c.concat_inline("_push rA", "_const")
    c.concat_raw(lo)
    c.concat_inline("_pullpc")


builtin_names = {
    "jmp": name_jump
}


class DiagnosticKind(StrEnum):
    Warn = "Warning"
    Err = "Error"
    Info = "Info"


class Compiler:
    def __init__(self, **kwargs):
        self.bytes = bytearray(0x200)
        self.diagnostics = []

        self.name_resolution_table = builtin_names
        self.label_offsets = {}
        self.unresolved_label_offsets = {}
        self.unresolved_label_offsets_hi = {}
        self.unresolved_label_offsets_lo = {}

        self.default_label_location = 0x200

    def concat_inline(self, *inline):
        for snippet in inline:
            self.concat_parsed_lines(parse(snippet))

    def concat_parsed_lines(self, lines: list[tuple]):
        for line in lines:
            match line:
                case (Line.Instruction, *instruction):
                    self.concat_instruction(instruction)
                case (Line.Name, name, *args):
                    self.try_concat_name(name, args)
                case (Line.Label, label):
                    self.insert_label(label)

    def concat_instruction(self, *instructions):
        for instruction in instructions:
            bs = op_to_bytes(instruction)
            if bs is None:
                self.warn(f"No instruction {instruction} found, ignoring")
                continue
            self.bytes.extend(bs)

    def concat_raw(self, *bs: int):
        for byte in bs:
            self.bytes.append(byte & 0xFF)

    def try_concat_name(self, name, args):
        if name not in self.name_resolution_table:
            self.warn(f"Could not find name `{name}`, inserting `_break`")
            self.concat_instruction((Op.Break,))
            return
        handler = self.name_resolution_table[name]
        handler(self)

    def concat_resolved_label(self, name):
        self.unresolved_label_offsets[self.offset] = name
        if name in self.label_offsets:
            self.concat_raw(0xCC, 0xCC)

    @property
    def offset(self) -> int:
        return len(self.bytes)

    def insert_label(self, label, args = {}):
        self.label_offsets[label] = self.offset

    def finalize(self) -> bytearray:
        out = self.bytes.copy()

        for offset, label in self.unresolved_label_offsets.items():
            if label_offset := self.label_offsets.get(label):
                hi, lo = hi_lo(label_offset)
                # TODO: verify the endianness
                self.bytes[offset] = hi
                self.bytes[offset + 1] = lo 
            else:
                self.warn(f"Label `{label}` was not resolved")

        return out

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

    c.concat_parsed_lines(lines)
    
    for _, line in c.diagnostics:
        print(line)
    
    return c.finalize()
