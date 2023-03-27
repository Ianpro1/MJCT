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
		.def(py::init<const char*, bool, double>(), py::arg("path"), py::arg("render") = false, py::arg("timestep") = 0.02)
		.def("step", &Tosser::step, py::arg("action"))
		.def("reset", &Tosser::reset)
		.def("render", &Tosser::render);

	/*
	//Tosser implemented using pybind11 wrapping over C++ function
	py::class_<TosserCPP>(m, "TosserCPP")
		.def(py::init<bool, double>(), py::arg("render") = false, py::arg("timestep") = 0.05)
		.def("reset", &Tosser::reset)
		.def("step", [](TosserCPP &t, py::array_t<double, py::array::c_style | py::array::forcecast> python_input) {
		py::buffer_info buffer_info = python_input.request();
		if (buffer_info.size != 2) {
			throw std::runtime_error("Input must be an array of shape (2,)");
		}
		double* action = static_cast<double*>(buffer_info.ptr);
		return t.step(action); })
		.def("render", [](TosserCPP &t) {
			unsigned char* rgb = t.render();
			py::array_t<unsigned char, py::array::c_style | py::array::forcecast> pixels({ 800, 800, 3 }, { 800 * 3, 3, 1 }, rgb);
			unsigned char* pixel_buffer = pixels.mutable_data();
			for (int i = 0; i < 800 / 2; i++) {
				std::swap_ranges(pixel_buffer + i * 800 * 3, pixel_buffer + (i + 1) * 800 * 3, pixel_buffer + (800 - i - 1) * 800 * 3);
			}
			return pixels;
			});*/
}
