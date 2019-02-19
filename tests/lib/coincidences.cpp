#include <array>
#include <numeric>
#include <random>
#include <iostream>

#include <generate.h>
#include <storage.h>

using namespace std;

pair<double, double> coincidence_rate(array<float, 4> rates) {

   Generators gens{1052, 9523, rates};

   long dt = std::lround(1e9);

   const size_t n_expect = gens.n_expect(dt);
   storage_t times; times.resize(n_expect);
   storage_t values; values.resize(n_expect);

   size_t idx = 0;
   long time_start = 0, time_end = 0;
   double av = 0.;
   const int n_iter = 10000;
   for (int i = 0; i < n_iter; ++i) {
      time_end += dt;
      auto n_coincidence = fill_coincidences(times, idx, time_start, time_end, gens);
      idx = 0;
      av += n_coincidence / double{n_iter};
      time_start = time_end;
   }

   return {gens.coincidence_rate, av};
}
