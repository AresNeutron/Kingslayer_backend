# King and knight are very easy
king_offsets = [
    (1, 0), (1, 1), (0, 1), (-1, 1),
    (-1, 0), (-1, -1), (0, -1), (1, -1)
]

knight_offsets = [
    (2, 1), (2, -1), (-2, 1), (-2, -1),
    (1, 2), (1, -2), (-1, 2), (-1, -2)
]

# This works for both kings and knights
def generate_moves(offsets: list) -> list:
    piece_moves = []  # Better be a list indexed by square

    for square in range(64):
        moves = 0  # Bitboard for piece's possible moves

        rank, file = divmod(square, 8)  # Convert 0-63 to (rank, file)

        for dr, df in offsets:
            new_rank = rank + dr
            new_file = file + df

            if 0 <= new_rank < 8 and 0 <= new_file < 8:  # Ensure inside board
                new_square = new_rank * 8 + new_file
                moves |= (1 << new_square)  # Set bit for valid move

        # Yes, better store them as a list
        piece_moves.append(moves)

    return piece_moves

# Generate and store knight moves
knight_lookup = generate_moves(knight_offsets)
king_lookup = generate_moves(king_offsets)
