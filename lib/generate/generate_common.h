/*
 * Copyright 2018-2019 NWO-I
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <vector>
#include <numeric>
#include <array>
#include <random>
#include "storage.h"

namespace Constants {

  // 2 * pi
  const float two_pi = 2.0 * 3.14159265358979323846;

  // parameters of probability parameterisation for coincidences
  // between PMTs as a function of their opening angle.
  const float p2 =  2.4347f;
  const float p3 = -0.68884f;
  const float p4 = 1.3911f;

  const int n_pmt = 31;

  // from fit to Gaussian part of ToT distribution
  const float tot_mean = 26.936f;
  const float tot_sigma = 2.44078f;

  const int random_method = 2;
  const std::array<float, 4> rates{7000., 700, 70., 7.};

  const float tau_l0 = 1.0e9f / rates[0];
}

float cross_prob(const float ct);

struct Generators {
private:
  const std::array<int, 2> m_seeds;
  const std::array<float, 4> m_rates{7000., 700, 70., 7.};
  std::seed_seq m_seed_seq;

public:
  Generators(const int seed0, const int seed1, const std::array<float, 4> rates);

  std::mt19937_64 mt;
  double coincidence_rate;
  std::geometric_distribution<long> coincidence;
  std::uniform_real_distribution<double> flat;
  std::array<float, 31> prob1D;
  std::array<std::array<float, 31>, 31> prob2D;
  std::array<float, 31> probND;

  const std::array<float, 4>& rates() const {
    return m_rates;
  }

  const std::array<int, 2>& seeds() const {
    return m_seeds;
  }

};

template<typename Container>
unsigned int random_index(const Container& buffer, const double random) {

  double x = std::accumulate(begin(buffer), end(buffer), 0.);

  if (x > 0.0) {
    x *= random;
    unsigned int index = 0;

    for (auto v : buffer) {
      if ((x -= v) < 0.0) {
        break;
      } else {
        ++index;
      }
    }

    if (index == buffer.size()) {
      --index;
    }
    return index;
  } else {
    throw std::domain_error("random_index(): zero or negative probability.");
  }
}

size_t fill_coincidences(storage_t& times, size_t idx, const long time_start, const long time_end,
                         Generators& gens);

std::tuple<storage_t, storage_t> generate(const long start, const long end,
                                          Generators& gens, bool use_avx2);
