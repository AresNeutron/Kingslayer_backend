// pawn.cpp
#include "data.h"

// Función que genera y retorna un std::array de movimientos de peón (avance)
std::array<uint64_t, 64> generate_pawn_moves_lookup(bool is_white) {
    std::array<uint64_t, 64> lookup_table{}; // Inicializa todos los elementos a 0
    
    for (int square = 0; square < 64; ++square) {
        uint64_t moves = 0ULL;
        int rank = square / 8;
        // int file = square % 8; // No necesario para movimientos de avance

        if (is_white) {
            // Avance de un paso (peón blanco)
            if (rank < 7) { // Asegura que no está en la última fila
                moves |= (1ULL << (square + 8)); // Adelante un paso
            }
            // Avance de dos pasos (solo desde la fila 2 para blanco)
            if (rank == 1) { // Si está en la segunda fila (índice 1)
                moves |= (1ULL << (square + 16));
            }
        } else { // Peón negro
            // Avance de un paso (peón negro)
            if (rank > 0) { // Asegura que no está en la primera fila
                moves |= (1ULL << (square - 8)); // Adelante un paso
            }
            // Avance de dos pasos (solo desde la fila 7 para negro)
            if (rank == 6) { // Si está en la séptima fila (índice 6)
                moves |= (1ULL << (square - 16));
            }
        }
        lookup_table[square] = moves;
    }
    return lookup_table;
}


// Función que genera y retorna un std::array de ataques de peón (diagonales)
std::array<uint64_t, 64> generate_pawn_attacks_lookup(bool is_white) {
    std::array<uint64_t, 64> lookup_table{}; // Inicializa todos los elementos a 0

    for (int square = 0; square < 64; ++square) {
        uint64_t attacks = 0ULL;
        int rank = square / 8;
        int file = square % 8;

        if (is_white) {
            // Ataques del peón blanco (diagonales)
            if (rank < 7) { // Asegura que no está en la última fila
                if (file > 0) { // Captura hacia la izquierda (columna > 0)
                    attacks |= (1ULL << (square + 7)); // Arriba-Izquierda
                }
                if (file < 7) { // Captura hacia la derecha (columna < 7)
                    attacks |= (1ULL << (square + 9)); // Arriba-Derecha
                }
            }
        } else { // Peón negro
            // Ataques del peón negro (diagonales)
            if (rank > 0) { // Asegura que no está en la primera fila
                if (file > 0) { // Captura hacia la izquierda (columna > 0)
                    attacks |= (1ULL << (square - 9)); // Abajo-Izquierda
                }
                if (file < 7) { // Captura hacia la derecha (columna < 7)
                    attacks |= (1ULL << (square - 7)); // Abajo-Derecha
                }
            }
        }
        lookup_table[square] = attacks;
    }
    return lookup_table;
}

// Peones: Movimientos de Avance
std::array<uint64_t, 64> white_pawn_moves_lookup;
std::array<uint64_t, 64> black_pawn_moves_lookup;

// Peones: Ataques (Capturas)
std::array<uint64_t, 64> white_pawn_attacks_lookup;
std::array<uint64_t, 64> black_pawn_attacks_lookup;


// Función específica para inicializar estas tablas
void init_pawn_lookups() {
    white_pawn_moves_lookup = generate_pawn_moves_lookup(true);
    black_pawn_moves_lookup = generate_pawn_moves_lookup(false);
    white_pawn_attacks_lookup = generate_pawn_attacks_lookup(true);
    black_pawn_attacks_lookup = generate_pawn_attacks_lookup(false);
}
