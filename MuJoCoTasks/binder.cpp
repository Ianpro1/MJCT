#include <pybind11/pybind11.h>
#include <mjct.h>


Tosser make(bool render) {
	Tosser env(render);
	return env;
}


PYBIND11_MODULE(make, m) {

	m.doc() = "makes a tosser environment";

	m.def("add", &make, "A function that creates a tosser environment");
}
