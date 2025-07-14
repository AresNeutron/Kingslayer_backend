// masks_blockers.cpp

#include <vector>
#include <cstdint> // Para uint64_t
#include <algorithm> // Para std::min
#include "utils.h"

 uint64_t generate_rook_mask(int square) {
    int rank = square / 8;
    int file = square % 8;
    uint64_t mask = 0ULL;

    for (int f = 1; f < 7; ++f) {
        if (f != file) { // Evita la propia posición de la torre
            mask |= (1ULL << (rank * 8 + f));
        }
    }

    for (int r = 1; r < 7; ++r) {
        if (r != rank) { // Evita la propia posición de la torre
            mask |= (1ULL << (r * 8 + file));
        }
    }

    return mask;
}


uint64_t generate_bishop_mask(int square) {
    int rank = square / 8;
    int file = square % 8;
    uint64_t mask = 0ULL;

    // Diagonal ↘↖ (hacia la derecha-abajo y la izquierda-arriba, excluyendo los bordes)
    // Hacia la esquina inferior derecha
    for (int i = 1; i < std::min(8 - file, 8 - rank); ++i) { // Asegura que no va más allá del borde
        mask |= (1ULL << ((rank + i) * 8 + (file + i)));
    }
    // Hacia la esquina superior izquierda
    for (int i = 1; i < std::min(file + 1, rank + 1); ++i) { // Asegura que no va más allá del borde
        mask |= (1ULL << ((rank - i) * 8 + (file - i)));
    }

    // Diagonal ↙↗ (hacia la izquierda-abajo y la derecha-arriba, excluyendo los bordes)
    // Hacia la esquina inferior izquierda
    for (int i = 1; i < std::min(file + 1, 8 - rank); ++i) { // Asegura que no va más allá del borde
        mask |= (1ULL << ((rank + i) * 8 + (file - i)));
    }
    // Hacia la esquina superior derecha
    for (int i = 1; i < std::min(8 - file, rank + 1); ++i) { // Asegura que no va más allá del borde
        mask |= (1ULL << ((rank - i) * 8 + (file + i)));
    }

    return mask;
}


std::vector<uint64_t> generate_blocker_combinations(uint64_t mask) {
    std::vector<uint64_t> combinations;

    std::vector<int> blocker_positions;
    for (int bit_pos = 0; bit_pos < 64; ++bit_pos) {
        if ((mask >> bit_pos) & 1ULL) { // Verificar si el bit está activado
            blocker_positions.push_back(bit_pos);
        }
    }

    int num_blockers = static_cast<int>(blocker_positions.size()); // Número total de bits relevantes en la máscara

    // Iterar sobre todas las combinaciones posibles (desde 0 hasta 2^num_blockers - 1)
    // 1ULL << num_blockers asegura que el 1 sea un unsigned long long antes del desplazamiento
    for (uint64_t i = 0; i < (1ULL << num_blockers); ++i) {
        uint64_t combination = 0ULL; // Bitboard para la combinación actual de bloqueadores

        for (int j = 0; j < num_blockers; ++j) {
            // Si el j-ésimo bit del contador 'i' está activado,
            // significa que el bloqueador correspondiente a blocker_positions[j] está presente.
            if ((i >> j) & 1ULL) {
                combination |= (1ULL << blocker_positions[j]); // Activar el bit para esa posición
            }
        }
        combinations.push_back(combination);
    }

    return combinations;
}


uint64_t generate_raw_sliding_attacks(int square, uint64_t occupancy,
                                      const std::vector<std::pair<int, int>>& directions) {
    int rank = square / 8;
    int file = square % 8;
    uint64_t attacks = 0ULL;

    for (const auto& dir : directions) {
        int dr = dir.first;
        int df = dir.second;
        int r = rank;
        int f = file;

        while (true) {
            r += dr;
            f += df;

            // Si sale del tablero, detén el rayo
            if (!(r >= 0 && r < 8 && f >= 0 && f < 8)) {
                break;
            }

            int pos = r * 8 + f;
            attacks |= (1ULL << pos); // Añade la casilla al bitboard de ataques

            // Si hay una pieza en esta casilla (del 'occupancy' dado), bloquea el rayo
            if ((occupancy >> pos) & 1ULL) {
                break;
            }
        }
    }
    return attacks;
}