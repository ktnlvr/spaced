from typing import Optional

from parser import Op, ArgKind


register_indexes = {"X": 0, "Y": 1, "AL": 2, "AH": 3, "A": 4, "F": 5, "PL": 6, "PH": 7}

b = lambda *s: bytes(s)


def op_to_bytes(instruction: list) -> Optional[bytes]:
    match (*instruction,):
        case (Op.Const, (ArgKind.Constant, n)):
            return b(0b00_000_010, n & 0xFF)
        case (Op.Copy, (ArgKind.Register, dst), (ArgKind.Register, src)):
            r1 = register_indexes[dst]
            r2 = register_indexes[src]
            return b(0b11_000_000 | ((r1 & 0b111) << 3) | (r2 & 0b111))
        case (Op.Add, (ArgKind.Register, r)):
            r = register_indexes[r]
            return b(0b01_000_000 | (r & 0b111))
        case (Op.Swap, (ArgKind.Register, r1), (ArgKind.Register, r2)):
            r1 = register_indexes[r1]
            r2 = register_indexes[r2]
            return b(0b10_000_000 | ((r1 & 0b111) << 3) | (r2 & 0b111))
        case (Op.JumpIfZero, (ArgKind.Register, r)):
            r = register_indexes[r]
            return b(0b00_110_000 | (r & 0b111))
        case (Op.Push, (ArgKind.Register, r)):
            r = register_indexes[r]
            return b(0b00_100_000 | (r & 0b111))
        case (Op.Break,):
            return b(0b00_000_001)
    return None
