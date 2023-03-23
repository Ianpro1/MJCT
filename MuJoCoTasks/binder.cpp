#include <pybind11/pybind11.h>
#include<test.h>

PYBIND11_MODULE(mjct, m) {

	m.doc() = "Pybind11Module";

	m.def("add", &say_hello, "A function that creates a tosser environment");
}
