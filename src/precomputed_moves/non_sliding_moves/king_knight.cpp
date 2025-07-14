// king_knight.cpp
#include <vector>
#include <utility>   // Para std::pair
#include "data.h"

// Los desplazamientos (offsets) del rey y del caballo se representan como pares (delta rank, delta file)
const std::vector<std::pair<int, int>> king_offsets = {
    {1, 0}, {1, 1}, {0, 1}, {-1, 1},
    {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
};

const std::vector<std::pair<int, int>> knight_offsets = {
    {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
    {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
};

// El tamaño del array (64) debe ser conocido en tiempo de compilación.
std::array<uint64_t, 64> generate_moves_lookup(const std::vector<std::pair<int, int>>& offsets) {
    std::array<uint64_t, 64> piece_moves_lookup{}; // Inicialización a ceros, importante
                                                  // {}: inicializa todos los elementos a cero

    for (int square = 0; square < 64; ++square) {
        uint64_t moves = 0ULL; // Usar 0ULL para asegurar que es un unsigned long long

        int rank = square / 8;
        int file = square % 8;

        for (const auto& offset : offsets) {
            int dr = offset.first;
            int df = offset.second;

            int new_rank = rank + dr;
            int new_file = file + df;

            // Asegurarse de que la nueva casilla esté dentro del tablero (0-7 para rank y file)
            if (new_rank >= 0 && new_rank < 8 && new_file >= 0 && new_file < 8) {
                int new_square = new_rank * 8 + new_file;
                moves |= (1ULL << new_square); // Establecer el bit para el movimiento válido
            }
        }
        piece_moves_lookup[square] = moves; // Asignar directamente al índice del array
    }
    return piece_moves_lookup;
}

std::array<uint64_t, 64> king_lookup;
std::array<uint64_t, 64> knight_lookup;

// Función específica para inicializar estas tablas
void init_king_knight_lookups() {
    knight_lookup = generate_moves_lookup(knight_offsets);
    king_lookup = generate_moves_lookup(king_offsets);
}