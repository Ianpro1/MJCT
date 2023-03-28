#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <mjct_py.h>
#include <mjct.h>
#include <mujoco/mujoco.h>

namespace py = pybind11;


//most of the important binding implementation were made directly onto the classes (will try to later implement their equivalent here)
PYBIND11_MODULE(mujocotasks, m) {

	//Tosser implemented using python binded function
	py::class_<Tosser>(m, "Tosser")
		.def(py::init<const char*, bool, double, double>(), py::arg("path"), py::arg("render") = false, py::arg("timestep") = 0.002, py::arg("apirate") = 100)
		.def("step", &Tosser::step, py::arg("action"))
		.def("reset", &Tosser::reset)
		.def("render", &Tosser::render);

	
	//Tosser implemented using pybind11 wrapping over C++ function
	//TODO add to make
	//TODO add fps to action
	py::class_<TosserCPP>(m, "TosserCPP")
		.def(py::init<const char*, bool, double, double>(), py::arg("path"), py::arg("render") = false, py::arg("timestep") = 0.002, py::arg("apirate") = 100)
		.def("reset", &TosserCPP::reset)
		.def("step", [](TosserCPP &t, py::array_t<double, py::array::c_style | py::array::forcecast> python_input) {
		py::buffer_info buffer_info = python_input.request();
		if (buffer_info.size != 2) {
			throw std::runtime_error("Input must be an array of shape (2,)");
		}
		double* action = static_cast<double*>(buffer_info.ptr);
		return t.step(action); })
		.def("render", &TosserCPP::render);
}
