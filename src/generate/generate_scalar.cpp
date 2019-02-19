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
#include <cassert>
#include <stdexcept>

#include <tuple>
#include <storage.h>
#include <generate_common.h>
#include <generate_scalar.h>

namespace {
// from fit to Gaussian part of ToT distribution
  const float tot_mean = Constants::tot_mean;
  const float tot_sigma = Constants::tot_sigma;

  using namespace storage;
  using std::cout;
  using std::endl;
}

float GenScalar::dot_product(const std::array<float, 3>& left, const std::array<float, 3>& right) {
  float r = 0.f;
  for (size_t i = 0; i < left.size(); ++i) {
    r += left[i] * right[i];
  }
  return r;
}

std::tuple<storage_t, storage_t> generate_scalar(const long time_start, const long time_end,
                                                 Generators& gens) {

  std::uniform_int_distribution<long> flat_pmt(0, 31);
  auto& mt = gens.mt;
  auto& flat = gens.flat;

  const size_t n_expect = gens.n_expect(time_end - time_start);
  const float tau_l0 = gens.tau_l0();

  storage_t times; times.resize(n_expect);
  storage_t values; values.resize(n_expect);
  times[0] = 0l;

  size_t idx = 0;

  // First generate some data
  for (int dom = 0; dom < storage::n_dom; ++dom) {
    for (int mod = 0; mod < storage::n_mod; ++mod) {
      size_t mod_start = idx;

      long offset = 0l;
      long last = 0l;
      while(last < time_end && idx < times.size() - 2) {
        // Generate times
        float r = -1.f * tau_l0 * log(flat(mt));
        last += static_cast<long>(r + 0.5);
        times[idx++] = last;
      }

      // Coincidences
      fill_coincidences(times, idx, time_start, time_end, gens);

      // fill values
      const size_t value_end = idx + (2 - (idx % 2));

      for (size_t vidx = mod_start; vidx < value_end; vidx += 2) {
        long pmt1 = flat_pmt(mt);
        long pmt2 = flat_pmt(mt);

        auto u1 = flat(mt);
        auto u2 = flat(mt) * Constants::two_pi;

        auto fact = sqrt(-2.0f * log(u1));
        float z0 = sin(u2);
        float z1 = cos(u2);

        z0 = fact * tot_sigma * z0 + tot_mean;
        z1 = fact * tot_sigma * z1 + tot_mean;

        auto val0 = static_cast<int>(z0) | (pmt1 << 8) | ((100 * (dom + 1) + mod + 1) << 13);
        values[idx - 1] = val0;

        auto val1 = static_cast<int>(z1) | (pmt2 << 8) | ((100 * (dom + 1) + mod + 1) << 13);
        values[idx] = val1;
      }
    }
  }
  times.resize(idx);
  values.resize(idx);
  return {std::move(times), std::move(values)};
}
