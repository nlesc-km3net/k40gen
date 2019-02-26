#include <array>
#include <numeric>
#include <random>
#include <iostream>
#include <unordered_map>

#include <generate.h>
#include <storage.h>

using namespace std;

tuple<double, double, unordered_map<size_t, double>>
coincidence_rate(array<float, 4> rates) {

   Generators gens{1052, 9523, rates};

   long dt = std::lround(1e7);

   const size_t n_expect = gens.n_expect(dt);
   storage_t times; times.resize(n_expect);
   storage_t values; values.resize(n_expect);

   long time_start = 0, time_end = 0;
   double av = 0.;
   const int n_iter = 10000;
   unordered_map<size_t, double> count;

   for (int i = 0; i < n_iter; ++i) {
      time_end += dt;
      auto [pmts, n_coincidence] = fill_coincidences(times, 0ul, time_start, time_end, gens);
      count[n_coincidence] += 1 / double{n_iter};
      av += n_coincidence / double{n_iter};
      time_start = time_end;
   }

   return {gens.coincidence_rate, av, count};
}
