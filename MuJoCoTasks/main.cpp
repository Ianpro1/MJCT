#include <mjct.h>
#include <pybind11/pybind11.h>
#include <GLFW/glfw3.h>

int finish(const char* error) {

	std::cout << error;
	return -1;
}

int main()
{

    std::string current_file(__FILE__);
    std::size_t last_separator = current_file.find_last_of("/\\");
    std::string current_directory = current_file.substr(0, last_separator);
    std::cout << "Current directory: " << current_directory << std::endl;
    return 0;


}