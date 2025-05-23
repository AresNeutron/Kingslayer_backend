from server.app.helpers.bitboards import bitboard_to_squares
from server.app.helpers.get_threats import get_threats
from server.app.data.bb_state import BBState, PAWN_IDX, KING_IDX
from server.app.main.move_getter import get_legal_moves
from typing import Tuple

"""Esta funciones pueden ser llamadas únicamente después de hecho un movimiento, no antes"""

def is_promotion(bb_state: BBState, is_white: bool) -> (int | None):
    """Verifica si un peón ha alcanzado la última fila para promoción."""
    pawn_idx = (PAWN_IDX + 6) if is_white else PAWN_IDX
    pawn_bb = bb_state.types_list[pawn_idx]

    # The last row if white, the first if black
    row = (pawn_bb >> 56) if is_white else (pawn_bb & 0b11111111)

    # There can be either one pawn in the row or none
    if row:
        return bitboard_to_squares(row)[0]
    else:
        return None
    

def is_check(bb_state: BBState, is_white: bool) -> Tuple[int, int] :
    """Verifica si el rey de este color está en jaque"""
    king_idx = (KING_IDX + 6) if is_white else KING_IDX
    king_bb = bb_state.types_list[king_idx]
    king_sq = bitboard_to_squares(king_bb)[0]

    threats = get_threats(bb_state, king_sq, not is_white)

    return threats, king_bb


# This can be pretty slow, but let's test it
def is_game_over(bb_state: BBState, is_white: bool) -> str:
    """Devuelve "checkmate", "stalemate" o "none"."""
    # 1) Localizar el rey
    king_idx = (KING_IDX + 6) if is_white else KING_IDX
    king_bb = bb_state.types_list[king_idx]
    king_sq = bitboard_to_squares(king_bb)[0]

    # 3) Contar atacantes para optimización double-check
    attackers = bitboard_to_squares(get_threats(bb_state, king_sq, not is_white))
    double_check = len(attackers) >= 2

    # 4) Probar movimientos legales
    # 4.a) Si double_check → sólo rey
    if double_check:
        moves = get_legal_moves(bb_state, king_sq)
        if moves:
            return "none"   # sí tiene al menos 1 movimiento rey
        return "checkmate"  # rey en jaque doble y sin escapatoria

    # 4.b) Jaque simple o sin jaque → probar piezas aliadas
    # Armamos un bitboard con todas las piezas propias
    friendly_bb = bb_state.get_friendly_bb(king_sq)
    # Iteramos sobre cada pieza aliada
    for sq in bitboard_to_squares(friendly_bb):
        legal = get_legal_moves(bb_state, sq)
        if legal:  # si hay al menos un destino legal
            return "none"

    # 5) No hay movimientos: decidimos mate o ahogado
    return "checkmate" if len(attackers) > 0 else "stalemate"
