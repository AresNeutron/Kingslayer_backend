#include "BoardState.h"
#include <cstdint>

// =========================
// BOARD MANIPULATION - OPTIMIZED
// =========================
void BoardState::setStartPosition() {
    board = INITIAL_MAILBOX_BOARD;
    types_bb_array = INITIAL_PIECE_BITBOARDS;
    occupied_bb = INITIAL_OCCUPANCY_ALL;
    colors_bb_array = INITIAL_OCCUPANCY_BY_COLOR;
}

void BoardState::movePiece(int fromSq, int toSq) {
    const Piece pc = board[fromSq];
    const Color color = colorOf(pc);
    
    const uint64_t fromMask = 1ULL << fromSq;
    const uint64_t toMask = 1ULL << toSq;
    const uint64_t moveMask = fromMask | toMask;
    
    // Update piece bitboard: clear from, set to
    types_bb_array[pc] ^= moveMask;
    
    // Update color bitboard: clear from, set to
    colors_bb_array[color] ^= moveMask;
    
    // Update occupancy: clear from, set to
    occupied_bb ^= moveMask;
    
    // Update mailbox
    board[fromSq] = NO_PIECE;
    board[toSq] = pc;
}

void BoardState::addPiece(int sq, Piece pc) {
    if (pc == NO_PIECE) return;

    // if (getType(pc) == KING) {
    //     std::cout << "You cannot ADD KING" << std::endl;
    //     std::cout << "Piece: " << static_cast<int>(pc) << std::endl;
    //     std::cout << "Square: " << sq << std::endl;
    //     return;
    // }

    const uint64_t mask = 1ULL << sq;
    const Color color = colorOf(pc);
    
    // Update all bitboards
    types_bb_array[pc] |= mask;
    colors_bb_array[color] |= mask;
    occupied_bb |= mask;
    
    // Update mailbox
    board[sq] = pc;
}

Piece BoardState::deletePiece(int sq) {
    const Piece pc = board[sq];

    if (getType(pc) == KING) {
        std::cout << "You cannot DELETE KING" << std::endl;
        std::cout << "Piece: " << static_cast<int>(pc) << std::endl;
        std::cout << "Square: " << sq << std::endl;
        return pc;
    }
    
    const uint64_t mask = 1ULL << sq;
    const Color color = colorOf(pc);
    
    // Update all bitboards
    types_bb_array[pc] &= ~mask;
    colors_bb_array[color] &= ~mask;
    occupied_bb &= ~mask;
    
    // Update mailbox
    board[sq] = NO_PIECE;
    
    return pc;
}

// delete all prints in this function
void BoardState::castling(int from_sq, int to_sq, bool reverse) {
    Piece king = reverse ? piece_at(to_sq) : piece_at(from_sq);
    Color sideToMove = colorOf(king);

    int rook_from = NO_SQ, rook_to = NO_SQ;

    if (to_sq - from_sq == +2) { // Enroque corto (King-side)
        if (sideToMove == WHITE) {
            rook_from = SQ_H1;  // 7
            rook_to   = SQ_F1;  // 5
        } else { // Caso de las negras
            rook_from = SQ_H8;  // 63
            rook_to   = SQ_F8;  // 61
        }
    } else if (to_sq - from_sq == -2) { // Enroque largo (Queen-side)
        if (sideToMove == WHITE) {
            rook_from = SQ_A1;  // 0
            rook_to   = SQ_D1;  // 3
        } else { // Caso de las negras
            rook_from = SQ_A8;  // 56
            rook_to   = SQ_D8;  // 59
        }
    } 

    if (reverse) {
        movePiece(rook_to, rook_from);
        movePiece(to_sq, from_sq);
    } else {
        movePiece(from_sq, to_sq);
        movePiece(rook_from, rook_to);
    }
}


void BoardState::promote(int promotion_sq, Type promotion) { // promotes to queen by default
    Piece pc = piece_at(promotion_sq);
    Type type = getType(pc);
    Color color = colorOf(pc);

    if (type != PAWN || !((PROMOTION_ROWS[color] >> promotion_sq) & 1ULL)) return;

    deletePiece(promotion_sq);

    Piece promotion_pc = static_cast<Piece>(color * PC_NUM + promotion);
    addPiece(promotion_sq, promotion_pc);
}