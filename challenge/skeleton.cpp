#include <array>
#include <vector>
#include <random>
#include <map>

#include <range/v3/core.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/algorithm/sort.hpp>

#include <generate.h>

namespace {

template <std::size_t N>
struct get_n {
  template <typename T>
  auto operator()(T&& t) const ->
    decltype(std::get<N>(std::forward<T>(t))) {
    return std::get<N>(std::forward<T>(t));
  }
};

const float PI = 3.1415927;

using namespace ranges;

using std::array;
using std::map;
using std::vector;

}
array<array<float, 3>, 3> pmt_rotation(float, float)
{
  return {{{1.f, 0.f, 0.f},
           {0.f, 1.f, 0.f},
           {0.f, 0.f, 1.f}}};
}

array<float, 3> rotate(const array<array<float, 3>, 3>& rotation,
                       const array<float, 3>& pmt)
{
  return pmt;
}


int main() {

  // Part 1: parse km3net_reference.detx into storage

  // Example storage of PMT postions
  map<int, array<float, 3>> PMTs;

  // Generate random hits
  // The rates are Hz per number of hits on neighbouring PMTs; this
  // needn't be changed.
  const array<float, 4> background_rates{7000.f, 700.f, 70.f, 7.f};
  Generators generators{41431, 2340, background_rates};

  // KM3NeT data arrives in time slices of 100ms. Here this data is
  // generated to a reasonable approximation of reality. How that
  // works is outside the scope of this challenge. Here we look at the
  // equivalent of 1 second of data.
  const size_t n_slices = 10;

  // Select 10 sets of hits per slice
  const size_t n_candidates = 10;

  // Generate slices of 100 ms (1e8 ns)
  const long dt = std::lround(1e8);

  for (size_t i = 0; i < n_slices; ++i) {
    // Values are three numbers packed into a single 32bit integer
    // - 8 bits: time over threshold of this hit (ToT)
    // - 5 bits: PMT_ID
    // - rest  : 100 * (DOM_ID + 1) + MOD_ID + 1
    // The ToT time and ToT are not relevant for this challenge
    auto [times, values] = generate(i * dt, (i + 1) * dt, generators, true);

    // Sort by time, this ensures there is some spread in PMTs within
    // clusters
    ranges::sort(view::zip(times, values), std::less<>{}, get_n<0>{});

    // Part 2: randomly select hits to use as candidates
    // Suggestion: use a normally distributed number of hits with a mean
    // of 20 an a width of 5, but no less than 5
    for (size_t candidate = 0; candidate < n_candidates; ++candidate) {
      size_t hit_start = 0, hit_end = 0;

      // Part 3: rotate selected hits over a grid of directions
      vector<array<float, 3>> result(hit_end - hit_start);

      float d_tp = 5.f * 2.f * PI / (360.f * 360.f);
      for (float theta = 0.f; theta < PI; theta += d_tp) {
        for (float phi = 0.f; phi < 2 * PI; phi += d_tp) {
          for (size_t i = hit_start; i < hit_end; ++i) {

            // Obtain PMT ID
            size_t pmt_id = 0;

            // Calculate rotation
            auto rotation = pmt_rotation(theta, phi);
            // Apply rotation
            result[i] = rotate(rotation, PMTs[pmt_id]);
          }
        }
      }
    }
  }
}
