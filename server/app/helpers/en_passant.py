from server.app.data.bb_state import BBState

# The current pawn shall match one of these tracks as well
CURRENT_WHITE_TRACK = 0b00000000_00000000_00000000_11111111_00000000_00000000_00000000_00000000
CURRENT_BLACK_TRACK = 0b00000000_00000000_00000000_00000000_11111111_00000000_00000000_00000000

BLACK_PAWN_IDX = 3
WHITE_PAWN_IDX = 9

def can_en_passant(bb_list: BBState, square: int):
    """Determina si un peón puede capturar al paso y devuelve el bitboard con la posición final."""
    # Verificar si el peón está en la fila correcta para capturar al paso
    is_white = bb_list.is_piece_white(square)
    
    current_track = CURRENT_WHITE_TRACK if is_white else CURRENT_BLACK_TRACK
    
    if not (current_track & (1 << square)):
        return 0

    en_passant_sq = bb_list.en_passant_sq
    
    # if en_passant_sq exists and one-step diagonal to the position of the own pawn
    if en_passant_sq is not None and abs(en_passant_sq % 8 - square % 8) == 1:
        return (1 << en_passant_sq)
    
    return 0
