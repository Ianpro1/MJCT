#include <mjct.h>

int main()
{
	FixedEnvironment env("tosser.xml");
	env.step();
	env.render();
	return 1;
}


