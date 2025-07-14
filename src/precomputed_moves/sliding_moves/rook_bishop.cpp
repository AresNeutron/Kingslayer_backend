// rook_bishop.cpp

#include "utils.h"
#include "data.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include <array>

void generate_magic_bitboards() {
    size_t current_rook_offset = 0;

    for (int square = 0; square < 64; ++square) {
        uint64_t mask = generate_rook_mask(square);
        rook_masks[square] = mask;
        
        int num_relevant_bits = __builtin_popcountll(mask);

        rook_magic_shifts[square] = 64 - num_relevant_bits;
        rook_magic_offsets[square] = static_cast<int>(current_rook_offset);

        std::vector<uint64_t> blocker_combinations = generate_blocker_combinations(mask);

        rook_magic_attack_table.resize(current_rook_offset + blocker_combinations.size());

        for (size_t i = 0; i < blocker_combinations.size(); ++i) {
            uint64_t relevant_blockers = blocker_combinations[i];
            
            uint64_t index = (relevant_blockers * ROOK_MAGICS[square]) >> (64 - num_relevant_bits);

            uint64_t actual_attacks = generate_raw_sliding_attacks(square, relevant_blockers, rook_directions);

            rook_magic_attack_table[current_rook_offset + index] = actual_attacks;
        }

        current_rook_offset += blocker_combinations.size();
    }

    size_t current_bishop_offset = 0;

    for (int square = 0; square < 64; ++square) {
        uint64_t mask = generate_bishop_mask(square);
        bishop_masks[square] = mask;

        int num_relevant_bits = __builtin_popcountll(mask);

        bishop_magic_shifts[square] = 64 - num_relevant_bits;
        bishop_magic_offsets[square] = static_cast<int>(current_bishop_offset);

        std::vector<uint64_t> blocker_combinations = generate_blocker_combinations(mask);

        bishop_magic_attack_table.resize(current_bishop_offset + blocker_combinations.size());

        for (size_t i = 0; i < blocker_combinations.size(); ++i) {
            uint64_t relevant_blockers = blocker_combinations[i];
            
            uint64_t index = (relevant_blockers * BISHOP_MAGICS[square]) >> bishop_magic_shifts[square];
            uint64_t actual_attacks = generate_raw_sliding_attacks(square, relevant_blockers, bishop_directions);

            bishop_magic_attack_table[current_bishop_offset + index] = actual_attacks;
        }

        current_bishop_offset += blocker_combinations.size();
    }
};

std::array<uint64_t, 64> rook_masks;
std::array<uint64_t, 64> bishop_masks;


std::array<int, 64> rook_magic_shifts;
std::array<int, 64> bishop_magic_shifts;


std::array<int, 64> rook_magic_offsets;
std::array<int, 64> bishop_magic_offsets;


std::vector<uint64_t> rook_magic_attack_table;
std::vector<uint64_t> bishop_magic_attack_table;