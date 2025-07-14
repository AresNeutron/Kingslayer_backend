#include <iostream>
#include <cstring>
#include <algorithm>
#include "./board_state/BoardState.h"
#include "./game/Game.h"

// Funciones auxiliares para las pruebas
bool compare_boards(const BoardState& b1, const BoardState& b2) {
    bool equal = true;
    if (memcmp(b1.board.data(), b2.board.data(), 64 * sizeof(Piece)) != 0) {
        std::cout << "FALLO: El 'board' no coincide." << std::endl;
        equal = false;
    }
    if (memcmp(b1.types_bb_array.data(), b2.types_bb_array.data(), (PC_NUM * 2) * sizeof(uint64_t)) != 0) {
        std::cout << "FALLO: 'types_bb_array' no coincide." << std::endl;
        equal = false;
    }
    if (b1.occupied_bb != b2.occupied_bb) {
        std::cout << "FALLO: 'occupied_bb' no coincide." << std::endl;
        equal = false;
    }
    if (memcmp(b1.colors_bb_array.data(), b2.colors_bb_array.data(), 2 * sizeof(uint64_t)) != 0) {
        std::cout << "FALLO: 'colors_bb_array' no coincide." << std::endl;
        equal = false;
    }
    return equal;
}

bool compare_game_states(const Game& g1, const Game& g2) {
    bool equal = true;
    if (!compare_boards(g1.board_state, g2.board_state)) {
        equal = false;
    }
    if (g1.castling_rights != g2.castling_rights) {
        std::cout << "FALLO: 'castling_rights' no coincide. Esperado: " << (int)g2.castling_rights << ", Obtenido: " << (int)g1.castling_rights << std::endl;
        equal = false;
    }
    if (g1.en_passant_sq != g2.en_passant_sq) {
        std::cout << "FALLO: 'en_passant_sq' no coincide. Esperado: " << (int)g2.en_passant_sq << ", Obtenido: " << (int)g1.en_passant_sq << std::endl;
        equal = false;
    }
    if (g1.sideToMove != g2.sideToMove) {
        std::cout << "FALLO: 'sideToMove' no coincide." << std::endl;
        equal = false;
    }
    if (g1.ply != g2.ply) {
        std::cout << "FALLO: 'ply' no coincide." << std::endl;
        equal = false;
    }
    return equal;
}
