from server.app.alpha_beta_prune.tables.tables import (
    PAWN_PSQ, KNIGHT_PSQ, KING_PSQ,
    ROOK_PSQ, BISHOP_PSQ, QUEEN_PSQ
)
from server.app.data.bb_state import (BBState,
    BISHOP_IDX, KING_IDX, KNIGHT_IDX,
    PAWN_IDX, QUEEN_IDX, ROOK_IDX )

PIECE_VALUES = {
    PAWN_IDX:   100,
    KNIGHT_IDX: 320,
    BISHOP_IDX: 330,
    ROOK_IDX:   500,
    QUEEN_IDX:  900,
    # rey: valor muy alto; no lo usamos directamente en la evaluación típica
}

# Primero, prepara un diccionario de PSQT completo:
PSQT = {}
for idx, table in [
    (PAWN_IDX,   PAWN_PSQ),
    (KNIGHT_IDX, KNIGHT_PSQ),
    (BISHOP_IDX, BISHOP_PSQ),
    (ROOK_IDX,   ROOK_PSQ),
    (QUEEN_IDX,  QUEEN_PSQ),
    (KING_IDX,   KING_PSQ),
]:
    PSQT[idx + 6] = table  # 6 is the adjustment for white pieces indexation
    PSQT[idx] = table[::-1] # 0 for black pieces

def evaluate_state(bb_state: BBState) -> int:
    score = 0

    # Material + PSQT
    for (color_adjust, sign) in ((6, +1), (0, -1)):
        for idx, base_value in PIECE_VALUES.items():
            bb = bb_state.types_list[idx + color_adjust]
            # material
            count = bb.bit_count()
            score += sign * count * base_value

            # posición
            x = bb
            while x:
                # LS1B
                sq = (x & -x).bit_length() - 1
                score += sign * PSQT[idx + color_adjust][sq]
                x &= x - 1  # clear LS1B

    return score
