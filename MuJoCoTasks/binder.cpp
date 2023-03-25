#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <test.h>
#include <mujoco/mujoco.h>

namespace py = pybind11;


//most of the important binding implementation were made directly onto the classes (will try to later implement their equivalent here)
PYBIND11_MODULE(mjct, m) {
	py::class_<Tosser>(m, "Tosser")
		.def(py::init<bool>(), py::arg("render"))
		.def("step", &Tosser::step, py::arg("action"))
		.def("reset", &Tosser::reset)
		.def("render", &Tosser::render);
}
