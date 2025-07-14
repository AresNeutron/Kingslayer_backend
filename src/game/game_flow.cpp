#include "Game.h"

void Game::user_moves(uint16_t move_code) {
    make_move(move_code);

    if (promotion_sq != NO_SQ) {
        std::cout << static_cast<int>(promotion_sq) << std::endl;
        std::cout << eventMessages[PROMOTION] << std::endl; // "promotion"
    } else {
        changeTurn();

        // these detectors can only be called in own turn
        uint64_t threats = detect_check();
        detect_game_over();

        std::cout << threats << std::endl;
        std::cout << eventMessages[game_event] << std::endl;
    }
    std::cout << "readyok\n";
}


void Game::resolve_promotion(int promotion) {
    board_state.promote(promotion_sq, static_cast<Type>(promotion));
    promotion_sq = NO_SQ;
    
    changeTurn();

    // these detectors can only be called in own turn
    uint64_t threats = detect_check();
    detect_game_over();

    std::cout << threats << std::endl;
    std::cout << eventMessages[game_event] << std::endl;
    std::cout << "readyok\n";
}
