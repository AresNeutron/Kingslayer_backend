from server.app.data.bb_state import BBState, KING_IDX, PAWN_IDX


def make_castling(bb_state: BBState, from_sq: int, to_sq: int):
    is_white = bb_state.is_piece_white(from_sq)

    # We need to know the positions of the rook to move it
    rook_from, rook_to = (
        (7, 5) if to_sq > from_sq else (0, 3)
    ) if is_white else (
        (63, 61) if to_sq > from_sq else (56, 59)
    )

    bb_state.move_bit(from_sq, to_sq) # Moving the king
    bb_state.move_bit(rook_from, rook_to)

    return bb_state


def make_en_passant(bb_state: BBState, from_sq: int, to_sq: int):
    is_white = bb_state.is_piece_white(from_sq)

    enemy_pawn_pos = (to_sq - 8) if is_white else (to_sq + 8)

    # First, delete the enemy pawn
    bb_state.move_bit(enemy_pawn_pos)

    # Move the selected pawn to its destinated square
    bb_state.move_bit(from_sq, to_sq)

    return bb_state


def make_normal_move(bb_state: BBState, from_sq: int, to_sq: int):
    enemy_bb = bb_state.get_enemy_bb(from_sq)

    # --- Handle capture ---
    if enemy_bb >> to_sq & 1:
        # By passing the "to_sq" in the place of "from_sq" it simply deletes the bit and that's it
        bb_state.move_bit(to_sq)
        
     # Update ally board and piece_type board: remove the piece in from_sq and add it in to_sq
    bb_state.move_bit(from_sq, to_sq)

    bb_state.en_passant_sq = None # clear the oportunity for en-passant after using 

    return bb_state


def make_move(bb_state: BBState, from_sq: int, to_sq: int):
    """ Realiza un movimiento y actualiza el estado del juego.
    Maneja movimientos normales, en passant, enroque y coronación. """

    bb_state.update_castling_rights(from_sq)
    bb_state.update_castling_rights(to_sq)

    is_white = bb_state.is_piece_white(from_sq)
    types_list = bb_state.types_list
    mask = 1 << from_sq

    is_king = bool(types_list[(KING_IDX + 6) if is_white else KING_IDX] & mask)
    is_pawn = bool(types_list[(PAWN_IDX + 6) if is_white else PAWN_IDX] & mask)

    if is_pawn:
        # en_passant execution, if possible
        if bb_state.en_passant_sq == to_sq:
            bb_state = make_en_passant(bb_state, from_sq, to_sq)
        else:
            bb_state = make_normal_move(bb_state, from_sq, to_sq)

            # en_passant detection
            if abs(to_sq - from_sq) == 16: # The pawn moved two steps
                bb_state.en_passant_sq = (to_sq + 8) if is_white else (to_sq - 8)

    else:
        bb_state.en_passant_sq = None # Clear the oportunity for en-passant if not used

        # if King moves more than one step, then it's castling
        if (is_king and abs(from_sq - to_sq) == 2):
            bb_state = make_castling(bb_state, from_sq, to_sq)

        else:
            bb_state = make_normal_move(bb_state, from_sq, to_sq)

    return bb_state