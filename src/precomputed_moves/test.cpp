#include "./non_sliding_moves/data.h"
#include "./sliding_moves/data.h"
#include <iostream>

int main () {
    init_king_knight_lookups();
    init_pawn_lookups();
    generate_magic_bitboards();

    std::cout << "Tablas generadas!" << std::endl;
}