#pragma once

#include <array>

std::pair<double, double> generate_times(std::array<float, 4> rates, bool use_avx2);

std::pair<double, double> coincidence_rate(std::array<float, 4> rates);
