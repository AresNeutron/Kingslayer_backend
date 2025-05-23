from server.app.data.non_sliding_pieces.king_and_knight import knight_lookup, king_lookup
from server.app.data.non_sliding_pieces.pawn import generate_pawn_moves
from server.app.data.sliding_pieces.moves import generate_sliding_moves

# Generar movimientos de peones
white_pawn_moves, black_pawn_moves, white_pawn_attacks, black_pawn_attacks = generate_pawn_moves()
BLACK_PAWN_IDX, WHITE_PAWN_IDX = 3, 9

# Generar movimientos de piezas deslizantes
bishop_masks, bishop_moves, rook_masks, rook_moves = generate_sliding_moves()

# It's a better practice to store constants with uppercase names
KNIGHT_LOOKUP = knight_lookup
KING_LOOKUP   = king_lookup
PAWN_MOVES    = { WHITE_PAWN_IDX: white_pawn_moves, 
                 BLACK_PAWN_IDX: black_pawn_moves }
PAWN_ATTACKS  = { WHITE_PAWN_IDX: white_pawn_attacks, 
                 BLACK_PAWN_IDX: black_pawn_attacks }
ROOK_MASKS    = rook_masks
ROOK_MOVES    = rook_moves
BISHOP_MASKS  = bishop_masks
BISHOP_MOVES  = bishop_moves