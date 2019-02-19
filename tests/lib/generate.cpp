#include <generate.h>
#include <storage.h>

#include <range/v3/view/zip.hpp>
#include <range/v3/algorithm/sort.hpp>

template <std::size_t N>
struct get_n {
  template <typename T>
  auto operator()(T&& t) const ->
    decltype(std::get<N>(std::forward<T>(t))) {
      return std::get<N>(std::forward<T>(t));
  }
};

using namespace std;

pair<double, double> generate_times(array<float, 4> rates, bool use_avx2) {

   Generators gens{1052, 9523, rates};

   long dt = std::lround(1e8);

   long time_start = 0, time_end = 0;
   time_end += dt;

   auto [times, values] = generate(time_start, time_end, gens, use_avx2);

   // Zip the ids and hits together to sort them together
   auto zipped = ranges::view::zip(times, values);

   // Sort the hits
   ranges::sort(zipped, std::less<long>{}, get_n<0>{});

   double av = 0.;
   const size_t n_times = times.size();
   for (size_t i = 0; i < n_times - 1; ++i) {
      av += static_cast<double>(times[i]) / n_times;
   }

   return {av, times.size()};
}
