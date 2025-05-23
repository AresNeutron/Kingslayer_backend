from server.app.data.precomputed_moves import (
    KNIGHT_LOOKUP, KING_LOOKUP,
    PAWN_MOVES, PAWN_ATTACKS,
    ROOK_MASKS, ROOK_MOVES,
    BISHOP_MASKS, BISHOP_MOVES,
)
from server.app.data.bb_state import (
    BISHOP_IDX, KING_IDX, KNIGHT_IDX,
    PAWN_IDX, ROOK_IDX
)
from server.app.data.bb_state import BBState
from server.app.helpers.castling import can_castle
from server.app.helpers.en_passant import can_en_passant
from server.app.helpers.get_threats import get_threats
from server.app.helpers.bitboards import bitboard_to_squares
from server.app.helpers.move_makers import make_move


# This function is very eficient, will be used a lot of times
def get_pseudo_legal_moves(bb_state: BBState, square: int) -> int:
    """
    Efficiently filter precomputed moves for sliding pieces using bitwise operations.
    """
    occupied    = bb_state.get_occupied_bb()
    friendly    = bb_state.get_friendly_bb(square)
    piece_idx = bb_state.get_piece_idx(square) # an integer from 0-11
    idx = piece_idx % 6 # an integer form 0-5
    moves = 0

    # Using if/elif block won't be slower than dictionary dispatch
    if idx  == KNIGHT_IDX:
        moves = KNIGHT_LOOKUP[square]
    elif idx == KING_IDX:
        moves = KING_LOOKUP[square]
    elif idx == PAWN_IDX:
        p_moves = PAWN_MOVES[piece_idx][square] & ~occupied
        p_attacks = PAWN_ATTACKS[piece_idx][square] & occupied
        p_attacks |= can_en_passant(bb_state, square)
            
        # This block is for cases when the pawn is in initial square and meets an obstancle just in front
        # Set moves to zero, since it can't move forward
        if ((piece_idx == 9 and (occupied >> (square + 8)) & 1)
                            or (piece_idx == 3 and (occupied >> (square - 8)) & 1)):
            p_moves = 0

        moves = p_moves | p_attacks
    elif idx == ROOK_IDX:
        mask  = ROOK_MASKS[square]
        idx   = occupied & mask
        moves = ROOK_MOVES[square].get(idx, 0)
    elif idx == BISHOP_IDX:
        mask  = BISHOP_MASKS[square]
        idx   = occupied & mask
        moves = BISHOP_MOVES[square].get(idx, 0)
    else:  # queen
        mask1 = ROOK_MASKS[square]
        idx1  = occupied & mask1
        mask2 = BISHOP_MASKS[square]
        idx2  = occupied & mask2
        moves = ROOK_MOVES[square].get(idx1,0) | BISHOP_MOVES[square].get(idx2,0)

    return moves & ~friendly


def get_legal_moves(bb_state: BBState, square: int) -> int:
    moves = get_pseudo_legal_moves(bb_state, square)

    """This is where the logic of simulation comes, 
    to filter those moves which put the king in danger"""
    legal_moves = 0

    is_white = bb_state.is_piece_white(square)
    curr_king_idx = (KING_IDX + 6) if is_white else KING_IDX

    for to_sq in bitboard_to_squares(moves):
        # Crear una copia del estado del tablero
        sim_bb_list = bb_state.copy()
        # El movimiento no se considera real hasta que se guarda en el GameManager
        sim_bb_list = make_move(sim_bb_list, square, to_sq)
        # 4.c) Obtener la casilla del rey tras el movimiento
        king_bb   = sim_bb_list.types_list[curr_king_idx]
        king_sq   = bitboard_to_squares(king_bb)[0]
        # 4.d) Si el rey NO está en jaque, es legal
        if not get_threats  (sim_bb_list, king_sq, not is_white):
            legal_moves |= (1 << to_sq)

    if bb_state.get_piece_idx(square) == curr_king_idx:
        legal_moves |= can_castle(bb_state, square)

    return legal_moves
