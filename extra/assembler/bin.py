from ops import Op, ArgKind

register_indexes = {"X": 0, "Y": 1, "AL": 2, "AH": 3, "A": 4, "F": 5, "PL": 6, "PH": 7}


def op_to_bytes(things: list) -> bytes:
    b = lambda s: bytes([s])

    # TODO: raise a warning if the constant does not fit
    match (*things,):
        case (Op.Const, (ArgKind.Constant, n)):
            return b(0b01_111_000 | (n & 0b111))
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
    return b(0)


def binarize(labels: dict[str, int], lines: dict) -> bytearray:
    out = bytearray(0x200)
    for _, line in lines.items():
        bs = op_to_bytes(line)
        out.extend(bs)
    return out