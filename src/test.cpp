#include "./search/Search.h"

int main () {
    init_king_knight_lookups();
    init_pawn_lookups();
    generate_magic_bitboards();
    init_ray_tables();

    Game game;
    Search search;

    // Till we can use a depth of 10, we won't stop, damn it!
    uint16_t result = search.find_best_move(game, 4);
    std::cout << "result: " << result << std::endl;

    game.get_board_state().printBoardArray();
    uint64_t occ = game.get_board_state().occupied();
    uint64_t white_bb = game.get_board_state().color_bb(WHITE);
    uint64_t black_bb = game.get_board_state().color_bb(BLACK);
    game.get_board_state().print_bitboard(occ);
    game.get_board_state().print_bitboard(white_bb);
    game.get_board_state().print_bitboard(black_bb);
}