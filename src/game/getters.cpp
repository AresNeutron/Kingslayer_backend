#include "Game.h"

std::vector<uint16_t> Game::get_legal_moves(int from_sq) {
    // Input validation
    if (from_sq < 0 || from_sq > 63) {
        std::cout << "Error, calling get_legal_moves with square number out of range" << std::endl;
        return {};
    }

    Piece piece = board_state.piece_at(from_sq);
    if (piece == NO_PIECE) {
        std::cout << "Error, calling get_legal_moves with an empty square" << std::endl;
        return {};
    }

    if (colorOf(piece) != sideToMove) {
        std::cout << "Error, attempting to call get_legal_moves function with a piece of the opposite turn" << std::endl;
        return {};
    }

    // Get pseudo-legal moves
    uint64_t pseudo_moves = board_state.getPseudoLegalMoves(from_sq);
    
    // Early exit if no pseudo-legal moves
    if (pseudo_moves == 0) { return {}; }

    // Initialize counters
    high_priority_count = 0;
    medium_priority_count = 0;
    low_priority_count = 0;

    Type piece_type = board_state.getType(piece);
    bool is_king = (piece_type == KING);
    bool is_pawn = (piece_type == PAWN);

    // Get king position and check status
    int king_sq = __builtin_ctzll(board_state.king(sideToMove));
    uint64_t king_attackers = board_state.getAttackersForSq(sideToMove, king_sq);
    int num_attackers = __builtin_popcountll(king_attackers);

    // Add en passant moves
    if (is_pawn && en_passant_sq != NO_SQ) {
        pseudo_moves |= get_en_passant_bb(from_sq);
    }

    // Calculate allowed destination squares in check
    uint64_t allowed_moves = calculate_allowed_moves_in_check(king_attackers, num_attackers);

    int move_count = 0;
    uint64_t enemy_bb = board_state.color_bb(static_cast<Color>(1 - sideToMove));

    // Process each pseudo-legal move
    while (pseudo_moves) {
        int to_sq = __builtin_ctzll(pseudo_moves);
        uint64_t to_sq_bb = 1ULL << to_sq;
        pseudo_moves &= pseudo_moves - 1; // Remove LSB

        // Skip if move is illegal
        if (!is_legal_move(from_sq, to_sq, piece, king_sq, num_attackers, allowed_moves, enemy_bb)) {
            continue;
        }

        // Determine move type and add to array
        MoveType move_type = get_move_type(from_sq, to_sq, piece, enemy_bb, to_sq_bb);
        uint16_t move_code = static_cast<uint16_t>((move_type << 12) | (from_sq << 6) | to_sq);
        prioritize_and_store_move(move_code);
    }

    // Handle castling for kings
    if (is_king && num_attackers == 0) {
        add_castling_moves(from_sq);
    }

    // Combine moves by priority
    std::vector<uint16_t> ordered_moves;
    ordered_moves.reserve(high_priority_count + medium_priority_count + low_priority_count);

    ordered_moves.insert(ordered_moves.end(), high_priority_moves.begin(), high_priority_moves.begin() + high_priority_count);
    ordered_moves.insert(ordered_moves.end(), medium_priority_moves.begin(), medium_priority_moves.begin() + medium_priority_count);
    ordered_moves.insert(ordered_moves.end(), low_priority_moves.begin(), low_priority_moves.begin() + low_priority_count);

    return ordered_moves;
}

// Calculate allowed moves when in check
uint64_t Game::calculate_allowed_moves_in_check(uint64_t king_attackers, int num_attackers) {
    if (num_attackers == 0) return ~0ULL; // No check
    if (num_attackers > 1) return 0ULL;   // Double check

    // Single check
    int enemy_sq = __builtin_ctzll(king_attackers);
    Piece enemy_piece = board_state.piece_at(enemy_sq);
    Type enemy_type = board_state.getType(enemy_piece);
    
    uint64_t allowed = king_attackers; // Capture attacker

    // Block sliding pieces
    if (enemy_type == BISHOP || enemy_type == ROOK || enemy_type == QUEEN) {
        allowed |= board_state.getRayBetween(sideToMove, enemy_sq);
    }

    return allowed;
}

// Check if a move is legal
bool Game::is_legal_move(int from_sq, int to_sq, Piece piece, int king_sq, 
                        int num_attackers, uint64_t allowed_moves, uint64_t enemy_bb) {
    Type piece_type = board_state.getType(piece);
    bool is_king = (piece_type == KING);
    uint64_t to_sq_bb = 1ULL << to_sq;

    // King moves: check if destination is attacked
    if (is_king) {
        if (num_attackers >= 1) { // in check
            uint64_t linear_attackers = board_state.getAttackersForSq(sideToMove, king_sq) 
            & board_state.getLinearThreats(sideToMove);

            while (linear_attackers) {
                int attacker_sq = __builtin_ctzll(linear_attackers);
                linear_attackers &= linear_attackers - 1;
                uint64_t attacker_ray = ray_between_table[to_sq][attacker_sq];

                if ((attacker_ray >> king_sq) & 1ULL) return false;
            }
        }

        return board_state.getAttackersForSq(sideToMove, to_sq) == 0;
    }

    // Non-king in double check cannot move
    if (num_attackers > 1) {
        return false;
    }

    // Check if move is on allowed squares in single check
    if (num_attackers == 1) {
        // En passant special case
        if (to_sq == en_passant_sq && piece_type == PAWN) {
            int captured_pawn_sq = sideToMove ? (to_sq - 8) : (to_sq + 8);
            uint64_t king_attackers = board_state.getAttackersForSq(sideToMove, king_sq);
            int attacker_sq = __builtin_ctzll(king_attackers);
            return (captured_pawn_sq == attacker_sq);
        }
        
        return (allowed_moves & to_sq_bb) != 0;
    }

    // Check pinned pieces
    if (pinned_rays[from_sq] != 0) {
        return (pinned_rays[from_sq] & to_sq_bb) != 0;
    }

    return true;
}

// Determine move type
MoveType Game::get_move_type(int from_sq, int to_sq, Piece piece, uint64_t enemy_bb, uint64_t to_sq_bb) {
    Type piece_type = board_state.getType(piece);
    bool is_capture = (enemy_bb & to_sq_bb) != 0;
    
    // En passant
    if (piece_type == PAWN && to_sq == en_passant_sq) {
        return EN_PASSANT;
    }
    
    // Promotion
    if (piece_type == PAWN && ((PROMOTION_ROWS[sideToMove] >> to_sq) & 1ULL)) {
        return is_capture ? PROMOTION_CAPTURE : PROMOTION;
    }
    
    // Regular move or capture
    return is_capture ? CAPTURE : MOVE;
}

// Add castling moves
void Game::add_castling_moves(int from_sq) {
    std::array<uint16_t, 2> castling_moves = get_castling_move(from_sq);
    for (int i = 0; i < 2; ++i) {
        if (castling_moves[i] != 0) {
            medium_priority_moves[medium_priority_count] = castling_moves[i];
            medium_priority_count++;
        }
    }
}

bool Game::would_be_check(uint16_t move_code) {
    // Data
    Color enemy_side = static_cast<Color>(1 - sideToMove);
    int enemy_king_sq = __builtin_ctzll(board_state.king(enemy_side));

    // Decompress move
    int from = (move_code >> 6) & 0b111111U;
    int to = move_code & 0b111111U;
    MoveType type = static_cast<MoveType>(move_code >> 12);

    if (board_state.getType(board_state.piece_at(to)) == KING) {
        std::cout << "Simulation with KING not allowed" << std::endl;
        return false;
    }

    bool is_check = false;
    bool is_capture = type == CAPTURE || type == PROMOTION_CAPTURE;
    Piece enemy = NO_PIECE;

    // Simulate move
    if (is_capture) enemy = board_state.deletePiece(to);

    board_state.movePiece(from, to);

    // Verify check
    is_check = board_state.getAttackersForSq(enemy_side, enemy_king_sq) > 0;

    // Reverse move
    board_state.movePiece(to, from);
    if (is_capture) board_state.addPiece(to, enemy);

    return is_check;
}

void Game::prioritize_and_store_move(uint16_t move_code) {
    // High priority: checks
    if (would_be_check(move_code)) {
        high_priority_moves[high_priority_count++] = move_code;
        return;
    }

    MoveType type = static_cast<MoveType>(move_code >> 12);

    // Medium priority: promotions
    if (type == PROMOTION || type == PROMOTION_CAPTURE) {
        medium_priority_moves[medium_priority_count++] = move_code;
        return;
    }

    // Evaluate captures
    if (type == CAPTURE) {
        int from_sq = (move_code >> 6) & 0x3F;
        Piece attacker_piece = board_state.piece_at(from_sq);
        Type attacker_type = board_state.getType(attacker_piece);

        int to_sq = move_code & 0x3F;
        Piece victim_piece = board_state.piece_at(to_sq);
        Type victim_type = board_state.getType(victim_piece);

        // Good capture: high priority
        if (PIECE_BASE_VALUE[victim_type] >= PIECE_BASE_VALUE[attacker_type]) {
            high_priority_moves[high_priority_count++] = move_code;
        }
        // Bad capture: low priority
        else {
            low_priority_moves[low_priority_count++] = move_code;
        }
        return;
    }
    
    // Low priority: en passant
    if (type == EN_PASSANT) {
        low_priority_moves[low_priority_count++] = move_code;
        return;
    }

    // Low priority: quiet moves
    low_priority_moves[low_priority_count++] = move_code;
}