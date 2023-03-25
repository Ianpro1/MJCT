#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <test.h>
#include <mujoco/mujoco.h>

namespace py = pybind11;


//most of mujoco's return type should use return_value_policy::reference since we don't want python to manipulate the memory (or copy in certain cases)
PYBIND11_MODULE(mjct, m) {
	py::class_<Tosser>(m, "Tosser")
		.def(py::init<bool>(), py::arg("render"))
		.def("step", &Tosser::step, py::arg("action"))
		.def("reset", &Tosser::reset);

	m.def("arr", &arr);
}

