#include "Game.h"
#include <cstdlib>
#include <iostream>


void Game::make_move(uint16_t move_code) {
    int from_sq = (move_code >> 6) & 0b111111U;
    int to_sq = move_code & 0b111111U;
    MoveType move_type = static_cast<MoveType>(move_code >> 12);
    
    Piece moving_piece = board_state.piece_at(from_sq);
    Piece captured_piece = NO_PIECE;
    int8_t new_ep_sq = NO_SQ;

    switch (move_type) {
        case MOVE: {
            // Check for double pawn push (creates en passant opportunity)
            if (board_state.getType(moving_piece) == PAWN && 
                std::abs(from_sq - to_sq) == 16) {
                new_ep_sq = sideToMove == WHITE ? (to_sq - 8) : (to_sq + 8);
            }
            board_state.movePiece(from_sq, to_sq);
            break;
        }
        
        case CAPTURE: {
            captured_piece = board_state.deletePiece(to_sq);
            board_state.movePiece(from_sq, to_sq);
            break;
        }
        
        case CASTLING: {
            board_state.castling(from_sq, to_sq);
            break;
        }
        
        case EN_PASSANT: {
            int captured_pawn_sq = sideToMove == WHITE ? (to_sq - 8) : (to_sq + 8);
            captured_piece = board_state.deletePiece(captured_pawn_sq);
            board_state.movePiece(from_sq, to_sq);
            break;
        }
        
        case PROMOTION: {
            board_state.movePiece(from_sq, to_sq);
            promotion_sq = to_sq;
            break;
        }
        
        case PROMOTION_CAPTURE: {
            captured_piece = board_state.piece_at(to_sq);
            board_state.deletePiece(to_sq);
            board_state.movePiece(from_sq, to_sq);
            promotion_sq = to_sq;
            break;
        }
        
        default: {
            std::cout << "Error, calling make_move function with an undefined move type: " 
                      << static_cast<int>(move_type) << std::endl;
            return;
        }
    }

    // Store current state for undo
    UndoInfo undo_info = {
        move_code, 
        captured_piece,
        en_passant_sq, 
        castling_rights
    };

    undo_stack[ply] = undo_info;
    en_passant_sq = new_ep_sq;
    update_castling_rights(to_sq);
    set_pinned_pieces();
}


void Game::unmake_move() {
    UndoInfo undo_info = undo_stack[ply];
    uint16_t move_code = undo_info.move_code;
    int from_sq = (move_code >> 6) & 0b111111U;
    int to_sq = move_code & 0b111111U;
    MoveType move_type = static_cast<MoveType>(move_code >> 12);
    
    // Restore move based on type (reverse of make_move)
    switch (move_type) {
        case MOVE: {
            board_state.movePiece(to_sq, from_sq);
            break;
        }
        
        case CAPTURE: {
            board_state.movePiece(to_sq, from_sq);
            board_state.addPiece(to_sq, undo_info.captured_piece);
            break;
        }
        
        case CASTLING: {
            board_state.castling(from_sq, to_sq, true);  // reverse = true
            break;
        }
        
        case EN_PASSANT: {
            int captured_pawn_sq = sideToMove == WHITE ? (to_sq - 8) : (to_sq + 8);
            board_state.movePiece(to_sq, from_sq);
            board_state.addPiece(captured_pawn_sq, undo_info.captured_piece);
            break;
        }
        
        case PROMOTION: {
            board_state.deletePiece(to_sq);
            Piece original_pawn = static_cast<Piece>(PAWN + (sideToMove * PC_NUM));
            board_state.addPiece(from_sq, original_pawn);
            break;
        }
        
        case PROMOTION_CAPTURE: {
            board_state.deletePiece(to_sq);
            board_state.addPiece(to_sq, undo_info.captured_piece);
            Piece original_pawn = static_cast<Piece>(PAWN + (sideToMove * PC_NUM));
            board_state.addPiece(from_sq, original_pawn);
            break;
        }
        
        default: {
            std::cout << "Error, attempting to call unmake_move function with undefined move type: " 
                      << static_cast<int>(move_type) << std::endl;
            return;
        }
    }

    // Restore game state
    castling_rights = undo_info.prev_castling_rights;
    en_passant_sq = undo_info.prev_en_passant_sq;
    promotion_sq = NO_SQ;
    set_pinned_pieces();
}


void Game::update_castling_rights(int to_sq) {
    Piece pc = board_state.piece_at(to_sq);
    Type type = board_state.getType(pc);
    // early exit if piece is neither rook nor king
    if (type != ROOK && type != KING) return;

    if (castling_rights == 0) return;

    switch (type)
    {
    case KING:{
        uint8_t remaining_rights = sideToMove ? 0b1100U : 0b0011U;
        castling_rights &= remaining_rights;
        }
        break;
    
    case ROOK:{
        int rook_idx = getCastlingIdx(to_sq);
        castling_rights &= ~(1U << rook_idx);
        }
        break;

    default:
        break;
    }
}