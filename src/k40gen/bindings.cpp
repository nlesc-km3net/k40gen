#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>

#include "xtensor/xmath.hpp"
#include "xtensor/xarray.hpp"

#define FORCE_IMPORT_ARRAY
#include "xtensor-python/pyarray.hpp"
#include "xtensor-python/pyvectorize.hpp"
#include "xtensor/xadapt.hpp"

#include <iostream>
#include <numeric>
#include <cmath>

#include <generate.h>
#include <storage.h>

namespace {
  namespace py = pybind11;
  using std::vector;
  using std::array;
}

inline xt::pyarray<long>
generate_k40(const long time_start, const long time_end, Generators& gens, bool avx2)
{
  auto r = generate(time_start, time_end, gens, avx2);
  auto times = std::get<0>(r);
  auto values = std::get<1>(r);
  auto n = times.size();

  // Create output array and copy timestamps into it
  vector<long> output(4 * n);
  std::copy(begin(times), end(times), begin(output));

  // factory for lambda's to shift and mask the values back into their
  // components.
  auto shift_mask_fact = [] (const size_t shift, const long mask) {
                            return [shift, mask] (const auto val) {
                                      return (val >> shift) & mask;
                                   };
                         };

  // column in the output array
  size_t column = 1;
  // Add DOM ID, PMT ID and TOT to the output array
  for (auto fun : {// DOM ID
                   shift_mask_fact(13, 0xffff),
                   // PMT ID
                   shift_mask_fact(8, 0x1f),
                   // TOT
                   shift_mask_fact(0, 0xff)}) {
    for (size_t i = 0; i < values.size(); ++i) {
      output[values.size() * column + i] = fun(values[i]);
    }
    ++column;
  }
  // Adapt to an xt::array to give to python
  return xt::adapt(std::move(output), array<size_t, 2>{4, n});
}

// Python Module and Docstrings
PYBIND11_MODULE(k40gen, m)
{
    xt::import_numpy();

    py::class_<Generators>(m, "Generators")
      .def(py::init<const uint, const uint, array<float, 4>>());


    m.doc() = R"pbdoc(
        standalong background generator for KM3NeT

        .. currentmodule:: k40gen

        .. autosummary::
           :toctree: _generate

           generate_k40
    )pbdoc";

    m.def("generate_k40", &generate_k40,
          py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>{},
          "Generate K40 background as a (4, n) array with columns: "
          "(time, DOM ID, PMT ID, time-over-threshold)");
}
