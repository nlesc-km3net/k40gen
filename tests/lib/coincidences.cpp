#include <array>
#include <numeric>
#include <random>
#include <iostream>
#include <unordered_map>

#include <generate.h>
#include <storage.h>

using namespace std;

tuple<double, double>
coincidence_rate(array<float, 4> rates) {

   Generators gens{1052, 9523, rates};
   auto l1_rate = std::accumulate(std::next(begin(rates)), end(rates), 0.);
   long dt = std::lround(1e9);

   storage_t times; times.resize(3 * l1_rate);
   storage_t values; values.resize(3 * l1_rate);
   pmts_t pmts(3 * l1_rate);

   long time_start = 0, time_end = 0;
   double av = 0.;
   const int n_iter = 5000;

   for (int i = 0; i < n_iter; ++i) {
      time_end += dt;
      auto [n_times, n_coincidences] = fill_coincidences(times, pmts, 0ul, time_start, time_end, gens);
      av += n_coincidences / double{n_iter};
      time_start = time_end;
   }
   return {gens.coincidence_rate, av};
}
