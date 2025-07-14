// PieceType.h

#pragma once
#include <cstdint>
#include <array>
#include <string>

// 0..5 = pieza negra (alfil, rey, caballo, peón, dama, torre)
// 6..11 = pieza blanca (alfil, rey, caballo, peón, dama, torre)
// 12 = casilla vacía
enum Piece : uint8_t {
    BLACK_BISHOP  = 0,
    BLACK_KING    = 1,
    BLACK_KNIGHT  = 2,
    BLACK_PAWN    = 3,
    BLACK_QUEEN   = 4,
    BLACK_ROOK    = 5,
    WHITE_BISHOP  = 6,
    WHITE_KING    = 7,
    WHITE_KNIGHT  = 8,
    WHITE_PAWN    = 9,
    WHITE_QUEEN   = 10,
    WHITE_ROOK    = 11,
    NO_PIECE      = 12
};

const std::string pieceStrings[] = {
    "bB", "bK", "bN", "bP", "bQ", "bR", // Piezas negras
    "wB", "wK", "wN", "wP", "wQ", "wR", // Piezas blancas
    "--" // NO_PIECE (puedes usar "  " o cualquier otra cosa)
};

// Para mapear pieza → índice “tipo” (0=alfil,1=rey,2=caballo,3=peón,4=dama,5=torre)
// independientemente de color:
enum Type : uint8_t {
    BISHOP  = 0,
    KING    = 1,
    KNIGHT  = 2,
    PAWN    = 3,
    QUEEN   = 4,
    ROOK    = 5,
};

enum Color : uint8_t {
    BLACK = 0,
    WHITE = 1,
};

// Para mapear pieza → color (0=negro, 1=blanco, 2=vacío)
inline Color colorOf(Piece pc) {
    if (pc <= BLACK_ROOK) return BLACK;
    return WHITE;
}

// Cantidad total de tipos “por color”: 6 (alfil, rey, caballo, peón, dama, torre).
static constexpr int PC_NUM = 6;

enum GameEvent : uint8_t {
    NONE = 0,
    CHECK = 1,
    CHECKMATE = 2,
    STALEMATE = 3,
    PROMOTION_EVENT = 4,
};

enum MoveType : uint8_t {
    MOVE = 0,
    CAPTURE = 1,
    CASTLING = 2,
    EN_PASSANT = 3,
    PROMOTION = 4,
    PROMOTION_CAPTURE = 5,
};

struct UndoInfo {
    uint16_t move_code;  // static_cast<uint16_t>((from_sq << 6) | to_sq);
    Piece captured_piece;
    int8_t prev_en_passant_sq;
    uint8_t prev_castling_rights;
};