from typing import List


# here we have to hardcode the access to the list, so this is the reference
BISHOP_IDX, KING_IDX, KNIGHT_IDX, PAWN_IDX, QUEEN_IDX, ROOK_IDX = 0, 1, 2, 3, 4, 5

# This is a constant, mostly used as reference for the indexes above
PIECE_TYPES = ['black_bishop', 'black_king', 'black_knight', 'black_pawn', 'black_queen',
                'black_rook', 'white_bishop', 'white_king', 'white_knight', 'white_pawn',
                'white_queen', 'white_rook']

TYPES_BITBOARDS = [
    0b00100100_00000000_00000000_00000000_00000000_00000000_00000000_00000000, # black_bishop
    0b00010000_00000000_00000000_00000000_00000000_00000000_00000000_00000000, # black_king
    0b01000010_00000000_00000000_00000000_00000000_00000000_00000000_00000000, # black_knight
    0b00000000_11111111_00000000_00000000_00000000_00000000_00000000_00000000, # black_pawn
    0b00001000_00000000_00000000_00000000_00000000_00000000_00000000_00000000, # black_queen
    0b10000001_00000000_00000000_00000000_00000000_00000000_00000000_00000000, # black_rook
    0b00000000_00000000_00000000_00000000_00000000_00000000_00000000_00100100, # white_bishop
    0b00000000_00000000_00000000_00000000_00000000_00000000_00000000_00010000, # white_king
    0b00000000_00000000_00000000_00000000_00000000_00000000_00000000_01000010, # white_knight
    0b00000000_00000000_00000000_00000000_00000000_00000000_11111111_00000000, # white_pawn
    0b00000000_00000000_00000000_00000000_00000000_00000000_00000000_00001000, # white_queen
    0b00000000_00000000_00000000_00000000_00000000_00000000_00000000_10000001,  # white_rook
]
COLORS_BITBOARDS = [
    0b11111111_11111111_00000000_00000000_00000000_00000000_00000000_00000000, # black_pieces
    0b00000000_00000000_00000000_00000000_00000000_00000000_11111111_11111111, # white_pieces
]

CASTLING_RIGHTS = 0b10010001_00000000_00000000_00000000_00000000_00000000_00000000_10010001


class BBState:
    def __init__(self):
        self.types_list: List[int] = list(TYPES_BITBOARDS)
        self.colors_list: List[int] = list(COLORS_BITBOARDS)
        self.castling_rights = CASTLING_RIGHTS
        self.en_passant_sq: int | None = None
    
    def is_piece_white(self, square: int):
            return bool((1 << square) & self.colors_list[1]) # indexed at the white pieces bitboard
        
    def get_occupied_bb(self):
        return (self.colors_list[0] | self.colors_list[1])
    
    def get_friendly_bb(self, square: int):
        mask = 1 << square

        return self.colors_list[0] if (self.colors_list[0] & mask) else self.colors_list[1]

    def get_enemy_bb(self, square: int):
        mask = 1 << square

        return self.colors_list[1] if (self.colors_list[0] & mask) else self.colors_list[0]
    
    def get_piece_idx(self, square: int):
        mask = 1 << square

        for idx in range(len(self.types_list)):
            if self.types_list[idx] & mask:
                return idx

    def _move_bit(self, bb_list: List[int], from_sq: int, to_sq: int | None = None):
        from_mask = 1 << from_sq
        to_mask = (1 << to_sq) if to_sq is not None else 0
        
        for i in range(len(bb_list)):
            # if this matches, the square is occupied
            if bb_list[i] & from_mask:
                # clear the bit in from_sq
                bb_list[i] &= ~from_mask
                # and activate it is to_sq
                bb_list[i] |= to_mask
                # Stop the loop now that change is done
                break

    def move_bit(self, from_sq: int, to_sq: int | None = None):
        self._move_bit(self.types_list, from_sq, to_sq) # sub-list with bb of piece types
        self._move_bit(self.colors_list, from_sq, to_sq) # sub-list with bb of both colors pieces

    def promote_pawn(self, square: int, promotion: str):
        idx = PIECE_TYPES.index(promotion)

        self._move_bit(self.types_list, square) # Delete the pawn from its bitboard
        self.types_list[idx] |= (1 << square) # Manually add it to the promotion bitboard

    # Let's assume that we call this only if a rook or a king moves, and before the move
    def update_castling_rights(self, square: int):
        mask = 1 << square

        if self.castling_rights & mask:
            self.castling_rights &= ~mask

    def copy(self) -> 'BBState':
        new_instance = self.__class__() # Crea una nueva instancia de la clase actual (BBList)
        
        # Copiar las listas. list() crea un nuevo objeto lista.
        new_instance.types_list = list(self.types_list)
        new_instance.colors_list = list(self.colors_list)
        
        # Copiar el entero (inmutable). La asignación basta.
        new_instance.castling_rights = self.castling_rights
        new_instance.en_passant_sq = self.en_passant_sq
        
        return new_instance