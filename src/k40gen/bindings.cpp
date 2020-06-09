#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <pybind11/numpy.h>

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

struct l0_t {
  long t;
  short dom_id;
  short pmt_id;
  short tot;
};

py::array_t<l0_t>
generate_k40(const long time_start, const long time_end, Generators& gens,
             std::string scheme, bool avx2)
{
  auto r = generate(time_start, time_end, gens, scheme, avx2);
  auto times = std::get<0>(r);
  auto values = std::get<1>(r);
  auto n = times.size();

  auto output = py::array_t<l0_t>(n);
  auto req = output.request();
  auto raw = static_cast<l0_t*>(req.ptr);

  // factory for lambda's to shift and mask the values back into their
  // components.
  auto shift_mask_fact = [] (const size_t shift, const long mask) {
                            return [shift, mask] (const auto val) {
                                      return (val >> shift) & mask;
                                   };
                         };

  // Add DOM ID, PMT ID and TOT to the output array
  auto value2dom_id = shift_mask_fact(13, 0xffff);
  auto value2pmt_id = shift_mask_fact(8, 0x1f);
  auto value2tot = shift_mask_fact(0, 0xff);

  for (size_t i = 0; i < times.size(); ++i) {
    auto const v = values[i];
    raw[i] = {times[i],
              static_cast<short>(value2dom_id(v)),
              static_cast<short>(value2pmt_id(v)),
              static_cast<short>(value2tot(v))};
  }

  return std::move(output);
}

// Python Module and Docstrings
PYBIND11_MODULE(k40gen, m)
{
  PYBIND11_NUMPY_DTYPE(l0_t, t, dom_id, pmt_id, tot);

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
