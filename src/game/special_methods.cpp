#include "Game.h"
#include <cstdlib>
#include <array>

uint64_t Game::get_en_passant_bb(int from_sq) const {
    uint64_t current_track = sideToMove ? WHITE_EN_PASSANT_TRACK : BLACK_EN_PASSANT_TRACK;
    
    if (!((current_track >> from_sq) & 1ULL)) {
        return 0;
    }

    bool isPinned = (pinned_rays[from_sq] != 0);
    uint64_t pin_ray = pinned_rays[from_sq];

    // en_passant_square es un int8_t, por lo que -1 o algún otro valor puede indicar que no hay captura al paso
    if (
        en_passant_sq != NO_SQ &&
        std::abs(en_passant_sq % 8 - from_sq % 8) == 1
    ) {
        if (isPinned) {
            return (pin_ray & en_passant_sq) ? (1ULL << en_passant_sq) : 0;
        }
        return (1ULL << en_passant_sq);
    }
    
    return 0;
}


std::array<int, 2> getCastlingPath(int rook_square) {
    switch (rook_square) {
        case 0:  return {2, 3}; // Torre en 0 para flanco de dama blanco
        case 7:  return {5, 6}; // Torre en 7 para flanco de rey blanco
        case 56: return {58, 59}; // Torre en 56 para flanco de dama negro
        case 63: return {61, 62}; // Torre en 63 para flanco de rey negro
    }
    return {NO_SQ, NO_SQ};
}


std::array<uint16_t, 2> Game::get_castling_move(int king_sq) const {
    uint8_t right_of_king_to_castle = sideToMove ? 0b0011U : 0b1100U;

    if (!(castling_rights & right_of_king_to_castle)) {
        return {0U,0U};
    }

    uint64_t occupied_bb = board_state.occupied();
    std::array<uint16_t, 2> castling_moves = {0U,0U};

    int q_rook_sq = sideToMove ? 0 : 56;
    int k_rook_sq = sideToMove ? 7 : 63;

    std::array<int, 2> rook_squares = {q_rook_sq, k_rook_sq};

    Piece rook_idx = static_cast<Piece>(ROOK + (sideToMove * PC_NUM));

    // # This one is to set the final position of the king
    int direction = 2;
    int index = 0;
    
    for (int rook_sq : rook_squares) {
        direction = -direction;

        int castling_idx = getCastlingIdx(rook_sq);

        // early exit if rook has already moved or if doesn't exists
        if (!((castling_rights >> castling_idx) & 1U) ||
            board_state.piece_at(rook_sq) != rook_idx) {
            continue;
        }
        
        std::array<int, 2> castling_squares = getCastlingPath(rook_sq);
        uint64_t castling_mask = (1ULL << castling_squares[0]) | (1ULL << castling_squares[1]);

        bool is_path_clear = (castling_mask & occupied_bb) == 0;

        bool is_king_safe = board_state.getAttackersForSq(sideToMove, king_sq) == 0;

        bool is_path_safe = true;

        for (int path_sq : castling_squares) {
            if (board_state.getAttackersForSq(sideToMove, path_sq) != 0ULL) {
                is_path_safe = false;
                break;
            }
        }

        if (is_king_safe && is_path_clear && is_path_safe) {
            uint16_t move_code = (CASTLING << 12) | (king_sq << 6) | (king_sq + direction);
            castling_moves[index] = move_code;
            index++;
        }
    }

    return castling_moves;
}


void Game::set_pinned_pieces() {
    pinned_rays.fill(0ULL);

    uint64_t threats = board_state.getLinearThreats(sideToMove);

    while (threats) {
        int threatSq = __builtin_ctzll(threats);
        threats &= threats - 1;
        
        uint64_t ray = board_state.getRayBetween(sideToMove, threatSq);

        uint64_t intersection = ray & board_state.color_bb(sideToMove);

        if (__builtin_popcountll(intersection) == 1) {
            int pinned_sq = __builtin_ctzll(intersection);

            pinned_rays[pinned_sq] = ray | (1ULL << threatSq);
        }
    }
}