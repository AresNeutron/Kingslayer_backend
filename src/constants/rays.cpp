#include "Rays.h"
#include <iostream>

std::array<std::array<uint64_t, 64>, 64> ray_between_table;

void init_ray_tables() {
    // Precompute rays between all square pairs
    for (int sq1 = 0; sq1 < 64; ++sq1) {
        for (int sq2 = 0; sq2 < 64; ++sq2) {
            if (sq1 == sq2) continue;
            
            const int r1 = sq1 / 8, f1 = sq1 % 8;
            const int r2 = sq2 / 8, f2 = sq2 % 8;
            
            // Check if squares are aligned (same rank, file, or diagonal)
            const int dr = (r2 > r1) ? 1 : ((r2 < r1) ? -1 : 0);
            const int df = (f2 > f1) ? 1 : ((f2 < f1) ? -1 : 0);
            
            // Only compute if aligned
            if (dr == 0 && df == 0) continue; // Same square
            if (dr != 0 && df != 0 && abs(r2 - r1) != abs(f2 - f1)) continue; // Not diagonal
            
            uint64_t ray = 0ULL;
            const int step = dr * 8 + df;
            
            for (int sq = sq1 + step; sq != sq2; sq += step) {
                ray |= 1ULL << sq;
            }
            
            ray_between_table[sq1][sq2] = ray;
        }
    }
}
