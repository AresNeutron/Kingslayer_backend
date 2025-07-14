// data.h
#ifndef PIECE_LOOKUP_TABLES_H
#define PIECE_LOOKUP_TABLES_H

#include <cstdint> // Para uint64_t
#include <array>   // Para std::array

// --- Tablas de Movimientos y Ataques Precomputados para Rey, Caballo y Peón ---

// Rey y Caballo: Movimientos
extern std::array<uint64_t, 64> king_lookup;
extern std::array<uint64_t, 64> knight_lookup;

// Peones: Movimientos de Avance
extern std::array<uint64_t, 64> white_pawn_moves_lookup;
extern std::array<uint64_t, 64> black_pawn_moves_lookup;

// Peones: Ataques (Capturas)
extern std::array<uint64_t, 64> white_pawn_attacks_lookup;
extern std::array<uint64_t, 64> black_pawn_attacks_lookup;


// --- Funciones de Inicialización ---
void init_king_knight_lookups();
void init_pawn_lookups();

#endif // PIECE_LOOKUP_TABLES_H