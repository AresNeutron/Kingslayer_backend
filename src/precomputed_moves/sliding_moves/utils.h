// utils.h

#ifndef UTILS_H
#define UTILS_H

#include <cstdint>   // Para uint64_t
#include <vector>    // Para std::vector
#include <utility>   // Para std::pair
#include <algorithm> // Para std::min
#include <iostream>

uint64_t generate_rook_mask(int square);

uint64_t generate_bishop_mask(int square);

std::vector<uint64_t> generate_blocker_combinations(uint64_t mask);

uint64_t generate_raw_sliding_attacks(int square, uint64_t occupancy,
                                      const std::vector<std::pair<int, int>>& directions);

const std::vector<std::pair<int, int>> rook_directions = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1} // Norte, Sur, Este, Oeste
};

const std::vector<std::pair<int, int>> bishop_directions = {
    {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // NE, NW, SE, SW
};


#endif // UTILS_H