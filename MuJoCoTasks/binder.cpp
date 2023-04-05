#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <mjct_py.h>
#include <mjct.h>
#include <mujoco/mujoco.h>
#include <pybind11/numpy.h>

namespace py = pybind11;


//most of the important binding implementation were made directly onto the classes (will try to later implement their equivalent here)
PYBIND11_MODULE(mujocotasks, m) {

	//Tosser implemented using python binded function
	//no pickling support
	py::class_<Tosser>(m, "Tosser")
		.def(py::init<const char*, bool, double, double>(), py::arg("path"), py::arg("render") = false, py::arg("timestep") = 0.002, py::arg("apirate") = 100)
		.def("step", &Tosser::step, py::arg("action"))
		.def("reset", &Tosser::reset)
		.def("render", &Tosser::render);

	
	//Tosser implemented using pybind11 wrapper over C++ function
	//add fps functionality
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
/*
		//pickling support (doesn't work for some reason?)

		.def(py::pickle(
			[](const TosserCPP& t) {
				//__getstate__
				auto params = py::array_t<double, 2>{ { static_cast<py::ssize_t>(t._params[0]), static_cast<py::ssize_t>(t._params[1]) } };
				auto path = std::string(t._path);
				return py::make_tuple(path, t.b_render, params);
			},
			[](py::tuple args) {
				//__setstate__
				if (args.size() != 4)
					throw std::runtime_error("Invalid state!");

				const char* path = args[0].cast<std::string>().c_str();
				double* params = args[2].cast<py::array_t<double, 2>>().mutable_data();

				//create new instance
				TosserCPP t(path, args[1].cast<bool>(), params[0], params[1]);

				//assign any additional state
				//no extra since functionality of environment isn't affected by parameters

				return t;
			}
			));*/
}
