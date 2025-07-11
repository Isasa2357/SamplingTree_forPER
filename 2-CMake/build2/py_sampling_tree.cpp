#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "SamplingTree.h"

namespace py = pybind11;

PYBIND11_MODULE(sampling_tree, m) {
    py::class_<SamplingTree>(m, "SamplingTree")
        .def(py::init<size_t>(), py::arg("capacity"))
        .def("add", &SamplingTree::add, py::arg("value"))
        .def("add_multiple", &SamplingTree::add_multiple, py::arg("values"))
        .def("get_smaples", &SamplingTree::get_smaples, py::arg("sample_size"))
        .def("update", &SamplingTree::update, py::arg("values"), py::arg("indics"))
        .def("total", &SamplingTree::total)
        .def("check_tree", &SamplingTree::check_tree, py::arg("err_threshold"))
        .def("max_err", &SamplingTree::max_err)
        .def("show", &SamplingTree::show)
        ;
}
