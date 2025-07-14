#include "Search.h"
#include <algorithm> // Para std::max
#include <limits> 

// Función principal que encuentra el mejor movimiento
uint16_t Search::find_best_move(Game& game, int depth) {
    uint16_t best_move = 0;
    double best_eval = -std::numeric_limits<double>::infinity();
    double alpha = -std::numeric_limits<double>::infinity();
    double beta = std::numeric_limits<double>::infinity();

    // Obtener todas las piezas del jugador actual
    uint64_t friendly_bb = game.get_board_state().color_bb(game.get_side_to_move());
    
    // Iterar sobre todas las piezas del jugador actual
    while (friendly_bb) {
        int from_sq = __builtin_ctzll(friendly_bb);
        friendly_bb &= friendly_bb - 1;
        // Obtener movimientos legales para esta pieza (ya ordenados por prioridad)
        std::vector<uint16_t> legal_moves = game.get_legal_moves(from_sq);
        
        // Procesar cada movimiento
        for (uint16_t move : legal_moves) {
            // Hacer el movimiento
            game.make_move(move);
            
            // Cambiar variables de estado
            game.changeTurn();
            game.increase_ply();
            
            // Llamada recursiva (negamax desde perspectiva del oponente)
            double eval = -negamax(game, depth - 1, -beta, -alpha);
            
            // Revertir el cambio antes de revertir movimiento
            game.decrease_ply();
            game.changeTurn();
            
            // Deshacer el movimiento
            game.unmake_move();
            
            // Actualizar el mejor movimiento
            if (eval > best_eval) {
                best_eval = eval;
                best_move = move;
            }
            
            // Actualizar alfa
            alpha = std::max(alpha, eval);
            
            // Poda alfa-beta (aunque es menos común en el nodo raíz)
            if (beta <= alpha) {
                break; // Poda beta
            }
        }
        
        // Si encontramos una poda, no necesitamos revisar más piezas
        if (beta <= alpha) {
            break;
        }
    }
    
    return best_move;
}

double Search::negamax(Game& game, int depth, double alpha, double beta) {
    // Caso base: profundidad 0 o fin del juego
    if (depth == 0 || game.detect_game_over()) {
        // Si es mate/tablas, manejar puntuaciones especiales
        if (game.detect_game_over()) {
            GameEvent event = game.get_game_event();
            if (event == CHECKMATE) {
                // Mate para el jugador actual es muy malo (-infinito)
                // Usar -10000 + depth para preferir mates más largos
                return -10000 + depth;
            } else if (event == STALEMATE) {
                // Tablas = 0
                return 0;
            }
        }
        
        // Evaluación normal del tablero
        return evaluate_board(game.get_board_state(), game.get_side_to_move());
    }
    
    double max_eval = -std::numeric_limits<double>::infinity();
    
    // Obtener todas las piezas del jugador actual
    uint64_t friendly_bb = game.get_board_state().color_bb(game.get_side_to_move());
    
    // Iterar sobre todas las piezas del jugador actual
    while (friendly_bb) {
        int from_sq = __builtin_ctzll(friendly_bb);
        friendly_bb &= friendly_bb - 1;
        // Obtener movimientos legales para esta pieza (ya ordenados por prioridad)
        std::vector<uint16_t> legal_moves = game.get_legal_moves(from_sq);
        
        // Procesar cada movimiento
        for (uint16_t move : legal_moves) {
            // Hacer el movimiento
            game.make_move(move);
            // Cambiar variables de estado
            game.changeTurn();
            game.increase_ply();
            
            // Llamada recursiva (negamax)
            double eval = -negamax(game, depth - 1, -beta, -alpha);
            
            // Revertir el cambio antes de revertir movimiento
            game.changeTurn();
            game.decrease_ply();

            // Deshacer el movimiento
            game.unmake_move();
            
            // Actualizar la mejor evaluación
            max_eval = std::max(max_eval, eval);
            
            // Actualizar alfa
            alpha = std::max(alpha, eval);
            
            // Poda alfa-beta
            if (beta <= alpha) {
                break; // Poda beta
            }
        }
        
        // Si encontramos una poda, no necesitamos revisar más piezas
        if (beta <= alpha) {
            break;
        }
    }
    
    return max_eval;
}


int Search::evaluate_board(const BoardState& board_state, Color sideToMove) const {
    int score = 0;
    
    uint64_t occupied_bb = board_state.occupied();
    while (occupied_bb) {
        int sq = __builtin_ctzll(occupied_bb);
        Piece pc = board_state.piece_at(sq);
        Type type = board_state.getType(pc); // Asumiendo que getType(pc) devuelve el tipo 0-5

        // Calcula el valor de la pieza en su casilla
        int piece_value = PIECE_BASE_VALUE[type] + get_table_by_piece(pc)[sq];

        // Si la pieza es blanca, suma. Si es negra, resta.
        if (colorOf(pc) == WHITE) {
            score += piece_value;
        } else {
            score -= piece_value;
        }
        
        occupied_bb &= occupied_bb - 1; // Avanza al siguiente bit
    }

    // Devuelve la puntuación desde la perspectiva del jugador actual.
    return (sideToMove == WHITE) ? score : -score;
}


void Search::engine_moves(Game& game) {
    // Depth of 4 is the max by now
    uint16_t best = find_best_move(game, 4);

    game.make_move(best);
    
    // later we must implement an heuristic for promotion
    if (game.get_promotion_sq() != NO_SQ) {
        game.get_board_state().promote(game.get_promotion_sq()); // promote to queen by default
        game.set_promotion_sq(NO_SQ);
    }

    game.changeTurn();

    uint64_t threats = game.detect_check();
    game.detect_game_over();

    std::cout << threats << std::endl;
    std::cout << eventMessages[game.get_game_event()] << std::endl;
    std::cout << "readyok\n";
}