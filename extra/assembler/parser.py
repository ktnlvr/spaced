from enum import StrEnum
from typing import Optional, Any


class Line(StrEnum):
    Label = "Label"
    Instruction = "Instruction"
    Name = "Name"


def parse_number(inp: str) -> Optional[int]:
    if inp.isnumeric():
        return int(inp)
    elif inp.startswith("0x"):
        return int(inp[2:], base=16)
    elif inp.startswith("0o"):
        return int(inp[2:], base=8)
    elif inp.startswith("0b"):
        return int(inp[2:], base=2)
    else:
        return None


def parse(input: str) -> list[Line]:
    lines = input.splitlines()
    out = []

    for i, line in enumerate(lines):
        line = line.strip()
        if not line:
            continue

        if line.startswith("--"):
            continue

        if line.startswith("."):
            label = line[1:]
            if label.endswith(":"):
                label = label[:-1]
            label = label.split()

            match label:
                case [
                    name,
                ]:
                    out.append((Line.Label, name))

            continue

        if line.startswith("_"):
            op, *args = line.split()
            out.append((Line.Instruction, str_to_op(op), *list(map(str_to_arg, args))))
            continue

        op, *args = line.split()
        out.append(
            (Line.Name, str_to_name(op), *list(map(str_to_arg, args)))
        )

    return out


class ArgKind(StrEnum):
    Register = "Register"
    Constant = "Constant"
    Label = "Label"


registers = ["rX", "rY", "rA", "rPL", "rPH", "rAL", "rAH"]


def str_to_arg(s) -> Optional[tuple[ArgKind, Any]]:
    if s in registers:
        return (ArgKind.Register, s[1:])
    if s.isnumeric():
        return (ArgKind.Constant, int(s))
    if s.startswith('.'):
        return (ArgKind.Label, s[1:])
    return None


class Op(StrEnum):
    Const = "_const"
    Copy = "_copy"
    Add = "_add"
    Swap = "_swap"
    JumpIfZero = "_jmpz"
    Push = "_push"
    Break = "_break"


def str_to_op(s) -> Optional[Op]:
    for op in Op:
        if str(op) == s:
            return op
    return None


def str_to_name(s) -> Optional[str]:
    return s
