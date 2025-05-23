from server.app.helpers.bitboards import bitboard_to_squares
from server.app.main.move_getter import get_legal_moves
from server.app.data.bb_state import BBState

def get_moves_for_color(bb_state: BBState, is_white: bool) -> list[int]:
    pieces_bb = bb_state.colors_list[1 if is_white else 0]
    moves = []
    for from_sq in bitboard_to_squares(pieces_bb):
        for to_sq in bitboard_to_squares(get_legal_moves(bb_state, from_sq)):
            moves.append((from_sq << 6) | to_sq)
    return moves
