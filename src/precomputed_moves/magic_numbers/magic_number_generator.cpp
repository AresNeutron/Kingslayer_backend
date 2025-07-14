#include <random>
#include <chrono>
#include "../sliding_moves/utils.h"
#include <algorithm> // Para std::fill
#include <vector>
#include <iostream>  // Para std::cout, std::endl
#include <iomanip>   // Para std::hex, std::setw, std::setfill
#include <array>

std::mt19937_64 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());

uint64_t generate_random_u64() {
    return rng();
}

uint64_t find_magic_number(int square, int num_relevant_bits, uint64_t mask,
                           const std::vector<std::pair<int, int>>& directions) {
    std::vector<uint64_t> blocker_combinations = generate_blocker_combinations(mask);
    std::vector<uint64_t> attacks_for_combinations;
    attacks_for_combinations.reserve(blocker_combinations.size());

    for (uint64_t combo : blocker_combinations) {
        attacks_for_combinations.push_back(generate_raw_sliding_attacks(square, combo, directions));
    }

    std::vector<uint64_t> used_attacks(1ULL << num_relevant_bits);

    while (true) {
        uint64_t magic = generate_random_u64();

        if (magic == 0ULL) continue;

        std::fill(used_attacks.begin(), used_attacks.end(), 0ULL);

        bool collision_found = false;
        for (size_t i = 0; i < blocker_combinations.size(); ++i) {
            uint64_t relevant_blockers = blocker_combinations[i];
            uint64_t current_attack_bitboard = attacks_for_combinations[i];

            uint64_t index = (relevant_blockers * magic) >> (64 - num_relevant_bits);

            if (used_attacks[index] == 0ULL) {
                used_attacks[index] = current_attack_bitboard;
            } else if (used_attacks[index] != current_attack_bitboard) {
                collision_found = true;
                break;
            }
        }

        if (!collision_found) {
            return magic;
        }
    }
}


int main() {
    // Arrays para almacenar los números mágicos
    std::array<uint64_t, 64> rook_magic_numbers;
    std::array<uint64_t, 64> bishop_magic_numbers;

    // 1. Generar y almacenar números mágicos para Torres
    std::cout << "Generando numeros magicos para Torres...\n";
    for (int square = 0; square < 64; ++square) {
        uint64_t mask = generate_rook_mask(square);
        int num_relevant_bits = __builtin_popcountll(mask);
        rook_magic_numbers[square] = find_magic_number(square, num_relevant_bits, mask, rook_directions);
        
        std::cout << "Rook - Square " << square << " generated.\n";
        std::cout << " Magic number: " << rook_magic_numbers[square] << std::endl;
    }
    std::cout << "Generacion de torres completada.\n\n";

    // 2. Generar y almacenar números mágicos para Alfiles
    std::cout << "Generando numeros magicos para Alfiles...\n";
    for (int square = 0; square < 64; ++square) {
        uint64_t mask = generate_bishop_mask(square);
        int num_relevant_bits = __builtin_popcountll(mask);
        bishop_magic_numbers[square] = find_magic_number(square, num_relevant_bits, mask, bishop_directions);

        std::cout << "Bishop - Square " << square << " generated.\n";
        std::cout << " Magic number: " << bishop_magic_numbers[square] << std::endl;
    }
    std::cout << "Generacion de alfiles completada.\n\n";

    // 3. Imprimir los arrays completos de números mágicos
    std::cout << "ROOK_MAGICS = {\n";
    for (int i = 0; i < 64; ++i) {
        std::cout << "    0x" << std::hex << std::setw(16) << std::setfill('0') << rook_magic_numbers[i];
        if (i < 63) {
            std::cout << ",";
        }
        std::cout << " // Square " << i << "\n";
    }
    std::cout << "};\n\n";

    std::cout << "BISHOP_MAGICS = {\n";
    for (int i = 0; i < 64; ++i) {
        std::cout << "    0x" << std::hex << std::setw(16) << std::setfill('0') << bishop_magic_numbers[i];
        if (i < 63) {
            std::cout << ",";
        }
        std::cout << " // Square " << i << "\n";
    }
    std::cout << "};\n" << std::endl;


    return 0;
}