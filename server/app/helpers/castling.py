from server.app.data.bb_state import BBState
from server.app.helpers.get_threats import get_threats

CASTLING_ROADMAP = {
    0: [2, 3], # The rook far from the white king
    7: [5, 6], # The rook close to the white king
    56: [58, 59], # The rook close from the black king
    63: [61, 62] # and so on
}

POSITIONS_MAP = { 4: (7, 0), 60: (63, 56) } # Rook postions based in king position


def can_castle(bb_state: BBState, king_square: int) -> int:
    """ Verifica si el enroque es posible para ambos lados. """
    # Early exit if king is not in its initial square
    if king_square not in POSITIONS_MAP.keys():
        return 0

    # or if it has alreay moved
    if not (1 << king_square) & bb_state.castling_rights:
        return 0

    occupied = bb_state.get_occupied_bb()
    castling_bitboard = 0

    # This one is to set the final position of the king
    direction = -2
    
    for rook_square in POSITIONS_MAP[king_square]:
        direction = -direction
        # Early exit if rook is not in its initial square
        if not (1 << rook_square) & bb_state.castling_rights:
            continue
        
        castling_squares = CASTLING_ROADMAP[rook_square]
        castling_mask = (1 << castling_squares[0]) | (1 << castling_squares[1])
        
        # No pieces between king and rook
        is_path_clear = (castling_mask & occupied) == 0

        # The squares the king's moves through are safe, and king is not in check
        castling_squares.append(king_square)
        is_path_safe = True
        
        # if any of them is threaten, set the variable to false
        for square in castling_squares:
            if bool(get_threats(bb_state, square, king_square == 60)):
                is_path_safe = False

        if is_path_clear and is_path_safe:
            castling_bitboard |= (1 << (king_square + direction))
    
    return castling_bitboard
