#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "xtensor/xmath.hpp"
#include "xtensor/xarray.hpp"

#define FORCE_IMPORT_ARRAY
#include "xtensor-python/pyarray.hpp"
#include "xtensor-python/pyvectorize.hpp"

#include <iostream>
#include <numeric>
#include <cmath>

#include <km3net/generate.h>

namespace py = pybind11;

inline xt::pyarray<long>
generate_k40(const long start, const long end, const Generators& gens, bool avx2)
{
  auto r = generate(start, end, gens, avx2);
  auto times = std::get<0>(r);
  auto values = std::get<1>(r);
  auto n = times.size();
  times.resize(2 * n);
  std::copy(begin(values), end(values), begin(times) + n);
  return xt::adapt(std::move(times), std::array<size_t>{2, n});
}

// Python Module and Docstrings
PYBIND11_MODULE(k40-gen, m)
{
    xt::import_numpy();

    py::class_<Generators>(m, "Generators")
      .def(py::init<const uint, const uint, std::array<float, 4>>());


    m.doc() = R"pbdoc(
        standalong background generator for KM3NeT

        .. currentmodule:: k40-gen

        .. autosummary::
           :toctree: _generate

           generate_k40
    )pbdoc";

    m.def("generate_k40", generate_k40, "Generate K40 background");
}
