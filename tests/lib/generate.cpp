#include <generate.h>
#include <storage.h>

#include <test_functions.h>

using namespace std;

pair<double, double> generate_l0(float l0_rate, long dt, bool use_avx2) {

   array<float, 4> rates = {l0_rate, 0., 0., 0.};
   Generators gens{1052, 9523, rates};

   long time_start = 0, time_end = 0;
   time_end += dt;

   auto [times, values] = generate(time_start, time_end, gens, use_avx2);

   double av = 0.;
   const size_t n_times = times.size();
   for (size_t i = 0; i < n_times - 1; ++i) {
      if (((values[i + 1]) >> 8) == (values[i] >> 8)) {
         av += static_cast<double>(times[i + 1] - times[i]) / n_times;
      }
   }

   return {av, times.size()};
}
