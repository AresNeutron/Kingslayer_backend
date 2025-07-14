#pragma once

#include <array>
#include <cstdint>
#include <cstdlib>

// Precomputed ray lookup tables - initialize once at startup
extern std::array<std::array<uint64_t, 64>, 64> ray_between_table;

void init_ray_tables();