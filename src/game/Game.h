#include "../board_state/BoardState.h"
#include "../constants/PSQ_tables.h"

#ifndef GAME_H
#define GAME_H

#include <cstdint>
#include <vector>
#include <cassert>


class Game {
private:
    // =========================
    // CORE GAME STATE
    // =========================
    BoardState board_state;
    uint8_t castling_rights;
    int8_t en_passant_sq;
    int8_t promotion_sq;
    Color sideToMove;
    GameEvent game_event;
    int ply;                     // Current search depth / move counter

    // =========================
    // SEARCH & MOVE DATA
    // =========================
    std::array<uint64_t, 64> pinned_rays;           // Ray for pinned pieces, 0 if not pinned
    std::array<UndoInfo, MAX_DEPTH> undo_stack;     // Stack for move undo information
   
    // three arrays to store different priority moves
    std::array<uint16_t, 8> high_priority_moves;
    std::array<uint16_t, 8> medium_priority_moves;  
    std::array<uint16_t, 24> low_priority_moves; 

    int high_priority_count, medium_priority_count, low_priority_count;

    // =========================
    // MOVE GENERATION HELPERS
    // =========================
    uint64_t get_en_passant_bb(int sq) const;
    std::array<uint16_t, 2> get_castling_move(int king_sq) const;
    void set_pinned_pieces();
    
    // Legal move generation helpers
    uint64_t calculate_allowed_moves_in_check(uint64_t king_attackers, int num_attackers);
    bool is_legal_move(int from_sq, int to_sq, Piece piece, int king_sq,
                       int num_attackers, uint64_t allowed_moves, uint64_t enemy_bb);
    MoveType get_move_type(int from_sq, int to_sq, Piece piece, uint64_t enemy_bb, uint64_t to_sq_bb);
    void add_castling_moves(int from_sq);
    bool would_be_check(uint16_t move_code);
    void prioritize_and_store_move(uint16_t move_code);

    // =========================
    // GAME STATE MANAGEMENT
    // =========================
    void update_castling_rights(int to_sq);

public:
    // =========================
    // CONSTRUCTOR
    // =========================
    Game() 
        : castling_rights(INITIAL_CASTLING_RIGHTS)
        , en_passant_sq(NO_SQ)
        , promotion_sq(NO_SQ)
        , sideToMove(WHITE)
        , game_event(NONE)
        , ply(0)
    {
        board_state = BoardState();
        pinned_rays.fill(0ULL);
    }

    // =========================
    // BASIC GAME CONTROL
    // =========================
    inline void changeTurn() noexcept { sideToMove = Color(1 - sideToMove); }
    inline void increase_ply() noexcept { ply++; }
    inline void decrease_ply() noexcept { ply--; }
    inline BoardState get_board_state() const noexcept { return board_state; }
    inline GameEvent get_game_event() const noexcept { return game_event; }
    inline Color get_side_to_move() const noexcept { return sideToMove; }
    inline int get_promotion_sq() const noexcept { return promotion_sq; }
    inline void set_promotion_sq(int8_t sq) noexcept { promotion_sq = sq; }

    // =========================
    // MOVE GENERATION & EXECUTION
    // =========================
    std::vector<uint16_t> get_legal_moves(int sq);
    void make_move(uint16_t move_code);
    void unmake_move();

    // =========================
    // GAME STATE ANALYSIS
    // =========================
    uint64_t detect_check();
    bool detect_game_over();

    // =========================P
    // USER INTERFACE METHODS
    // =========================
    void user_moves(uint16_t move_code);
    void resolve_promotion(int promotion);
};

#endif