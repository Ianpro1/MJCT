#include <mujoco/mujoco.h>
#include <GLFW/glfw3.h>
#include <iostream>


int finish(const char* msg = NULL, mjModel* m = NULL) {
	// deallocate model
	if (m) {
		mj_deleteModel(m);
	}

	// print message
	if (msg) {
		std::printf("%s\n", msg);
	}

	return 0;
}


int main() {
	char error[1000] = "Could not load binary model";
	mjModel* m = NULL;
	m = mj_loadXML("C:/Users/ianmi/Documents/MuJoCo/model/humanoid/humanoid.xml", 0, error, 1000);
	if (!m)
	{
		return finish(error);
	}




	return finish("done", m);
}