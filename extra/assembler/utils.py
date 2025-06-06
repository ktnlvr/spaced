def hi_lo(n: int) -> tuple[int, int]:
    return (n >> 8) & 0xFF, n & 0xFF
