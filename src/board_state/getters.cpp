#include "BoardState.h"
#include <cstdint>
#include <vector>

uint64_t BoardState::getAttackersForSq(Color sideToMove, int sq) const{
    Color oppColor = static_cast<Color>(1 - sideToMove); 
    int enemy_idx = oppColor * PC_NUM;

    uint64_t attackers = 0ULL;

    // --- Lookup precomputado (simétrico) ---
    attackers |= (types_bb_array[KING + enemy_idx] & king_lookup[sq]);
    attackers |= (types_bb_array[KNIGHT + enemy_idx] & knight_lookup[sq]);

    // Pawn attacks - optimized with direct lookup
    const uint64_t pawnAttackers = types_bb_array[PAWN + enemy_idx];
    if (oppColor == WHITE) {
        attackers |= pawnAttackers & black_pawn_attacks_lookup[sq];
    } else {
        attackers |= pawnAttackers & white_pawn_attacks_lookup[sq];
    }

    // --- Deslizadores: alfiles y reinas (diagonales) ---
    uint64_t bishop_mask = bishop_masks[sq];
    uint64_t occ_diag = occupied_bb & bishop_mask;
    uint64_t bishop_magic_index = (occ_diag * BISHOP_MAGICS[sq]) >> bishop_magic_shifts[sq];
    uint64_t bishop_attacks = bishop_magic_attack_table[bishop_magic_offsets[sq] + bishop_magic_index];
    attackers |= (bishop_attacks & (types_bb_array[BISHOP + enemy_idx] | types_bb_array[QUEEN + enemy_idx]));

    // --- Torres y reinas (líneas rectas) ---
    uint64_t rook_mask = rook_masks[sq];
    uint64_t occ_line = occupied_bb & rook_mask;
    uint64_t rook_magic_index = (occ_line * ROOK_MAGICS[sq]) >> rook_magic_shifts[sq];
    uint64_t rook_attacks = rook_magic_attack_table[rook_magic_offsets[sq] + rook_magic_index];;
    attackers |= (rook_attacks & (types_bb_array[ROOK + enemy_idx] | types_bb_array[QUEEN + enemy_idx]));

    return attackers;
}


uint64_t BoardState::getLinearThreats(Color sideToMove) const {
    Piece kingPiece = static_cast<Piece>(KING + sideToMove * PC_NUM); // index of the king
    uint64_t kingBB = types_bb_array[kingPiece];

    int kingSq = __builtin_ctzll(kingBB); // Posición del rey (asume único rey)

    int oppColor = 1 - sideToMove;  
    int enemy_idx = oppColor * PC_NUM;
    uint64_t enemy_bb = colors_bb_array[oppColor];

    uint64_t threats = 0ULL;

    // --- Deslizadores: alfiles y reinas (diagonales) ---
    uint64_t bishop_mask = bishop_masks[kingSq];
    uint64_t occ_diag = enemy_bb & bishop_mask;
    uint64_t bishop_magic_index = (occ_diag * BISHOP_MAGICS[kingSq]) >> bishop_magic_shifts[kingSq];
    uint64_t bishop_attacks = bishop_magic_attack_table[bishop_magic_offsets[kingSq] + bishop_magic_index];
    threats |= (bishop_attacks & (types_bb_array[BISHOP + enemy_idx] | types_bb_array[QUEEN + enemy_idx]));

    // --- Torres y reinas (líneas rectas) ---
    uint64_t rook_mask = rook_masks[kingSq];
    uint64_t occ_line = enemy_bb & rook_mask;
    uint64_t rook_magic_index = (occ_line * ROOK_MAGICS[kingSq]) >> rook_magic_shifts[kingSq];
    uint64_t rook_attacks = rook_magic_attack_table[rook_magic_offsets[kingSq] + rook_magic_index];;
    threats |= (rook_attacks & (types_bb_array[ROOK + enemy_idx] | types_bb_array[QUEEN + enemy_idx]));

    return threats;
}


uint64_t BoardState::getRayBetween(Color sideToMove, int sq) const {
    // Get king position
    const Piece kingPiece = static_cast<Piece>(KING + sideToMove * PC_NUM);
    const uint64_t kingBB = types_bb_array[kingPiece];
    const int king_sq = __builtin_ctzll(kingBB);
    
    // O(1) lookup instead of runtime calculation
    return ray_between_table[sq][king_sq];
}


uint64_t BoardState::getPseudoLegalMoves(int from_sq) const {
    Piece pc = piece_at(from_sq); // 0-11

    uint64_t occupied = occupied_bb;

    uint64_t friendly = getFriendlyBB(pc);

    Type type = getType(pc);

    uint64_t moves = 0;

    switch (type) {
        case KNIGHT:
            moves = knight_lookup[from_sq];
            break;
        case KING:
            moves = king_lookup[from_sq];
            break;
        case PAWN: {
            uint64_t p_moves = (colorOf(pc) ? white_pawn_moves_lookup[from_sq] : black_pawn_moves_lookup[from_sq]) & ~occupied;
            uint64_t p_attacks = (colorOf(pc) ? white_pawn_attacks_lookup[from_sq] : black_pawn_attacks_lookup[from_sq]) & occupied;

            // Obstáculo directo frente al peón
            if ((pc == WHITE_PAWN && (occupied >> (from_sq + 8)) & 1) ||
                (pc == BLACK_PAWN && (occupied >> (from_sq - 8)) & 1)) {
                p_moves = 0;
            }

            moves = p_moves | p_attacks;
            break;
        }
        case ROOK: {
            uint64_t mask = rook_masks[from_sq];
            uint64_t occupied_mask = occupied & mask;
            uint64_t magic_index = (occupied_mask * ROOK_MAGICS[from_sq]) >> rook_magic_shifts[from_sq];
            moves = rook_magic_attack_table[rook_magic_offsets[from_sq] + magic_index];
            break;
        }
        case BISHOP: {
            uint64_t mask = bishop_masks[from_sq];
            uint64_t occupied_mask = occupied & mask;
            uint64_t magic_index = (occupied_mask * BISHOP_MAGICS[from_sq]) >> bishop_magic_shifts[from_sq];
            moves = bishop_magic_attack_table[bishop_magic_offsets[from_sq] + magic_index];
            break;
        }
        case QUEEN: {
            uint64_t rook_mask = rook_masks[from_sq];
            uint64_t occupied_for_rook = occupied & rook_mask;
            uint64_t rook_magic_index = (occupied_for_rook * ROOK_MAGICS[from_sq]) >> rook_magic_shifts[from_sq];
            uint64_t rook_moves = rook_magic_attack_table[rook_magic_offsets[from_sq] + rook_magic_index];

            uint64_t bishop_mask = bishop_masks[from_sq];
            uint64_t occupied_for_bishop = occupied & bishop_mask;
            uint64_t bishop_magic_index = (occupied_for_bishop * BISHOP_MAGICS[from_sq]) >> bishop_magic_shifts[from_sq];
            uint64_t bishop_moves = bishop_magic_attack_table[bishop_magic_offsets[from_sq] + bishop_magic_index];

            moves = rook_moves | bishop_moves;
            break;
        }
    }

    return moves & ~friendly;
};
