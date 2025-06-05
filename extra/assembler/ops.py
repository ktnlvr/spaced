from enum import StrEnum
from typing import Optional, Any


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
    return None


class Op(StrEnum):
    Const = "_const"
    Copy = "_copy"
    Add = "_add"
    Swap = "_swap"
    JumpIfZero = "_jmpz"


def str_to_op(s) -> Optional[Op]:
    for op in Op:
        if str(op) == s:
            return op
    return None
