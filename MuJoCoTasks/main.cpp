#include <mjct.h>

void pause() {
	char input;
	std::cout << "enter any key value to continue...";
	std::cin >> input;
}

int finish(const char* error) {

	std::cout << error;
	pause();
	return -1;
}

int main()
{
	mjModel* m;
	char error[100] = "Could not load tosser.xml";
	m = mj_loadXML("batmobile.xml", 0, error, 100);
	if (!m) {
		return finish(error);
	}
	std::cout << m;
	pause();
}