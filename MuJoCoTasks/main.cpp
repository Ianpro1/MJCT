#include <mjct.h>


int main()
{
	Tosser env(false);
	double actions[] = { 0.3, 0.5 };
	env.step(actions);

	std::cout << "press any key to leave: ";
	int input;
	std::cin >> input;

}