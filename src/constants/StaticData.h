// InitialStateConsts.h
#pragma once

#include <array>
#include <cstdint> // Para uint64_t
#include "Types.h"

// Definiciones de los bitboards iniciales para cada tipo de pieza.
// El orden debe coincidir con el enum Piece (BLACK_BISHOP, BLACK_KING, ..., WHITE_ROOK)
// y BITBOARD_COUNT debe ser 12 (el número de tipos de piezas).
constexpr std::array<uint64_t, 12> INITIAL_PIECE_BITBOARDS = {
    0b0010010000000000000000000000000000000000000000000000000000000000ULL, // BLACK_BISHOP
    0b0001000000000000000000000000000000000000000000000000000000000000ULL, // BLACK_KING
    0b0100001000000000000000000000000000000000000000000000000000000000ULL, // BLACK_KNIGHT
    0b0000000011111111000000000000000000000000000000000000000000000000ULL, // BLACK_PAWN
    0b0000100000000000000000000000000000000000000000000000000000000000ULL, // BLACK_QUEEN
    0b1000000100000000000000000000000000000000000000000000000000000000ULL, // BLACK_ROOK
    0b0000000000000000000000000000000000000000000000000000000000100100ULL, // WHITE_BISHOP
    0b0000000000000000000000000000000000000000000000000000000000010000ULL, // WHITE_KING
    0b0000000000000000000000000000000000000000000000000000000001000010ULL, // WHITE_KNIGHT
    0b0000000000000000000000000000000000000000000000001111111100000000ULL, // WHITE_PAWN
    0b0000000000000000000000000000000000000000000000000000000000001000ULL, // WHITE_QUEEN
    0b0000000000000000000000000000000000000000000000000000000010000001ULL  // WHITE_ROOK
};

// Bitboards de ocupación por color para la posición inicial.
constexpr std::array<uint64_t, 2> INITIAL_OCCUPANCY_BY_COLOR = {
    0b1111111111111111000000000000000000000000000000000000000000000000ULL, // black_pieces (ranks 8 and 7)
    0b0000000000000000000000000000000000000000000000001111111111111111ULL  // white_pieces (ranks 2 and 1)
};

constexpr uint64_t WHITE_EN_PASSANT_TRACK = 0x000000FF00000000ULL; // Fila 4 para peones blancos
constexpr uint64_t BLACK_EN_PASSANT_TRACK = 0x00000000FF000000ULL; // Fila 5 para peones negros


// Bitboards para saber si un peón está promoviendo
constexpr std::array<uint64_t, 2> PROMOTION_ROWS = {
    0b0000000000000000000000000000000000000000000000000000000011111111ULL,
    0b1111111100000000000000000000000000000000000000000000000000000000ULL, 
};

// Bitboard de ocupación total para la posición inicial.
constexpr uint64_t INITIAL_OCCUPANCY_ALL =
    INITIAL_OCCUPANCY_BY_COLOR[0] | INITIAL_OCCUPANCY_BY_COLOR[1];

// Derechos de enroque iniciales (todos disponibles).
constexpr uint8_t INITIAL_CASTLING_RIGHTS = 0b1111U;

// Para el en passant, en la posición inicial no hay casilla de en passant.
constexpr int8_t NO_SQ = -1;

// Casillas de movimiento de enroque
static constexpr int SQ_E1 = 4, SQ_G1 = 6, SQ_C1 = 2, SQ_H1 = 7, SQ_A1 = 0;
static constexpr int SQ_E8 = 60, SQ_G8 = 62, SQ_C8 = 58, SQ_H8 = 63, SQ_A8 = 56;
static constexpr int SQ_F1 = 5, SQ_F8 = 61, SQ_D1 = 3, SQ_D8 = 59;


// Definiciones de piezas para llenar el tablero "mailbox" (array de 64 posiciones)
constexpr std::array<Piece, 64> INITIAL_MAILBOX_BOARD = {
    WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK, // Rank 1 (0-7)
    WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN,       // Rank 2 (8-15)
    NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE,       // Rank 3 (16-23)
    NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE,       // Rank 4 (24-31)
    NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE,       // Rank 5 (32-39)
    NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE,       // Rank 6 (40-47)
    BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN,       // Rank 7 (48-55)
    BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK  // Rank 8 (56-63)
};

constexpr std::array<const char*, 5> eventMessages = {
    "none", "check", "checkmate", "stalemate", "promotion"
};

constexpr int MAX_DEPTH = 5; // This is the max secure depth by now
constexpr int MAX_MOVES = 28; // This number is an ideal case: queen in the middle of an empty board


inline int getCastlingIdx(int rook_square) {
    switch (rook_square) {
        case 0:  return 0; // Torre en 0 para flanco de dama blanco
        case 7:  return 1; // Torre en 7 para flanco de rey blanco
        case 56: return 2; // Torre en 56 para flanco de dama negro
        case 63: return 3; // Torre en 63 para flanco de rey negro
    } 
    return NO_SQ;
}

constexpr int CHECK_BONUS = 1000;
constexpr int CHECKMATE_BONUS = 10000;

