from math import log2

def bitboard_to_squares(bitboard: int) -> list[int]:
    squares = []

    while bitboard:
        lowest_bit = bitboard & -bitboard
        squares.append(int(log2(lowest_bit)))
        bitboard = bitboard & (bitboard - 1)

    return squares


def print_bitboard(bitboard: int):
    """Helper function to visualize the board"""
    for rank in range(7, -1, -1):
        row = []
        for file in range(8):
            square = rank * 8 + file
            if bitboard & (1 << square):
                row.append("1 ")
            else:
                row.append(". ")
        print("".join(row))
    print()
