from server.app.data.precomputed_moves import (
    KNIGHT_LOOKUP, KING_LOOKUP,
    ROOK_MASKS, ROOK_MOVES,
    BISHOP_MASKS, BISHOP_MOVES,
)
from server.app.helpers.bitboards import bitboard_to_squares
from server.app.data.bb_state import (BBState,
    BISHOP_IDX, KING_IDX, KNIGHT_IDX,
    PAWN_IDX, QUEEN_IDX, ROOK_IDX)


# This is cool, and it's goddamn fast
def get_threats(bb_state: BBState, square: int, is_attacker_white: bool) -> int:
    """Devuelve lista de cuadrados de piezas enemigas que atacan a square."""
    attackers: int = 0

    occupied = bb_state.get_occupied_bb()
    types_list = bb_state.types_list
    king_bb = 1 << square
    color_index = 6 if is_attacker_white else 0

    # easy one first: king and knight, simetric attacks
    attackers |= (types_list[KING_IDX + color_index] & KING_LOOKUP[square])
    attackers |= (types_list[KNIGHT_IDX + color_index] & KNIGHT_LOOKUP[square])
    
    # pawns
    if is_attacker_white:
        # White pawns attack NE (+7) and NW (+9)
        attackers |= ((king_bb >> 9) & types_list[PAWN_IDX + color_index])
        attackers |= ((king_bb >> 7) & types_list[PAWN_IDX + color_index])
    else:
        # Black pawns attack SE (−7) and SW (−9)
        attackers |= ((king_bb << 9) & types_list[PAWN_IDX + color_index])
        attackers |= ((king_bb << 7) & types_list[PAWN_IDX + color_index])

    # bishop and queen, diagonal attacks
    idx_b = occupied & BISHOP_MASKS[square]
    attackers |= (BISHOP_MOVES[square].get(idx_b, 0)
        & (types_list[BISHOP_IDX + color_index] | types_list[QUEEN_IDX + color_index]))

    # rook and queen, horizontal/vertical attacks
    idx_r = occupied & ROOK_MASKS[square]
    attackers |= (ROOK_MOVES[square].get(idx_r, 0)
        & (types_list[ROOK_IDX + color_index] | types_list[QUEEN_IDX + color_index]))
    
    return attackers
