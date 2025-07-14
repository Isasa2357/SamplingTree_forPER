#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "SamplingTree.h"

namespace py = pybind11;

PYBIND11_MODULE(SamplingTree, m) {
    py::class_<SamplingTree>(m, "SamplingTree")
        .def(py::init<size_t>(), py::arg("capacity"))
        .def("add", &SamplingTree::add, py::arg("value"))
        .def("add_multiple", &SamplingTree::add_multiple, py::arg("values"))
        .def("get_smaples", &SamplingTree::get_smaples, py::arg("sample_size"))
        .def("update", &SamplingTree::update, py::arg("values"), py::arg("indics"))
        .def("total", &SamplingTree::total)
        .def("max_leaf", &SamplingTree::max_leaf)
        .def("check_tree", &SamplingTree::check_tree, py::arg("err_threshold"))
        .def("max_err", &SamplingTree::max_err)
        .def("check_max_leaf", &SamplingTree::check_max_leaf)
        .def("show", &SamplingTree::show)
        ;
}
