#include "Game.h"

// These funcions can be used only right after a move takes place


// Verifies if last enemy move put ally king in check
uint64_t Game::detect_check() {
    uint64_t king_bb = board_state.king(sideToMove);
    int king_sq = __builtin_ctzll(king_bb);

    uint64_t threats = board_state.getAttackersForSq(sideToMove, king_sq);
    if (threats != 0) {
        game_event = CHECK;
        threats |= king_bb;
    }else {
        game_event = NONE;
    }

    return threats;
}

// Verifies if last enemy move ended the game, only called when check was previously detected
bool Game::detect_game_over() {
    if (game_event != CHECK) return false;

    uint64_t king_bb = board_state.king(sideToMove);
    int king_sq = __builtin_ctzll(king_bb);

    uint64_t threats = board_state.getAttackersForSq(sideToMove, king_sq);
    bool double_check = (__builtin_popcountll(threats) >= 2);

    if (double_check) {
        std::vector<uint16_t> movesVector = get_legal_moves(king_sq);
        if (!movesVector.empty()) {
            return false; // No checkmate
        }   
        game_event = CHECKMATE; // double-check with no escape
        return true;
    }

    uint64_t friendly_bb = board_state.color_bb(sideToMove);

    while (friendly_bb) {
        int sq = __builtin_ctzll(friendly_bb);
        friendly_bb &= friendly_bb - 1;
        
        std::vector<uint16_t> movesVector = get_legal_moves(sq);
        if (!movesVector.empty()) {
            return false; // No checkmate
        }
    }
    
    if (threats > 0) {
        game_event = CHECKMATE; // King is in check and no legal moves for any piece
    } else {
        game_event = STALEMATE; // King is not in check, but no legal moves for any piece
    }
    return true;
}