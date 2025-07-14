#pragma once
#include "../board_state/BoardState.h"
#include "../game/Game.h"

class Search {
public:
    int evaluate_board(const BoardState& board_state, Color sideToMove) const;

    // Función Negamax con Poda Alfa-Beta
    double negamax(Game& game, int depth, double alpha, double beta); // Recibe una referencia a Game

    // Función principal para encontrar el mejor movimiento
    uint16_t find_best_move(Game& game, int depth);

    void engine_moves(Game& game);
};