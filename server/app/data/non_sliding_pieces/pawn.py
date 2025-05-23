# This was done earlier, I will assume it works
def generate_pawn_moves():
    # Again, list indexed by square are far better than dictionaries for performance
    white_pawn_moves = []
    black_pawn_moves = []

    white_pawn_attacks = []
    black_pawn_attacks = []

    for square in range(64):
        moves_w = 0  # White pawn moves
        moves_b = 0  # Black pawn moves
        attacks_w = 0  # White pawn attacks
        attacks_b = 0  # Black pawn attacks

        rank = square // 8  # Rank (0-7)
        file = square % 8  # File (0-7)

        # White pawn normal moves
        if rank < 7:
            moves_w |= (1 << (square + 8))  # One step forward
            if rank == 1:  # Two steps forward if on rank 2
                moves_w |= (1 << (square + 16))

        # Black pawn normal moves
        if rank > 0:
            moves_b |= (1 << (square - 8))  # One step forward
            if rank == 6:  # Two steps forward if on rank 7
                moves_b |= (1 << (square - 16))

        # White pawn attack moves (diagonals)
        if rank < 7:
            if file > 0:  # Capture left
                attacks_w |= (1 << (square + 7))
            if file < 7:  # Capture right
                attacks_w |= (1 << (square + 9))

        # Black pawn attack moves (diagonals)
        if rank > 0:
            if file > 0:  # Capture left
                attacks_b |= (1 << (square - 9))
            if file < 7:  # Capture right
                attacks_b |= (1 << (square - 7))

        white_pawn_moves.append(moves_w)
        black_pawn_moves.append(moves_b)
        white_pawn_attacks.append(attacks_w)
        black_pawn_attacks.append(attacks_b)

    return white_pawn_moves, black_pawn_moves, white_pawn_attacks, black_pawn_attacks

# Test only
# white_pawn_lookup, black_pawn_lookup, white_pawn_attacks, black_pawn_attacks = generate_pawn_moves()

# print("White Pawn from d2 (attacks):")
# print_board(white_pawn_attacks[11])

# print("Black Pawn from d7 (attacks):")
# print_board(black_pawn_attacks[51])
