//MuJoCo Documentation Summary

// in any simulation you will use void mj_step(const mjModel* m, mjData* d);
// where mjData is the dynamic state of our model which is constructed at runtime while mjModel by the compiler

// mj_makeData is to receive mjData of model which can be deleted using mj_deleteData
// mj_deleteModel also

// each model has 3 set of options: mjOption (physics), mjVisual, mjStatistic (model properties); specified in XML file if not -> default

//Assets: Mesh. Skin, Skin, Height field, texture, Material

//Kinematic trees: are the dynamic rigid bodies whose motion is constrained
//body, (has mass and inertial properties but no geometric properties, instead those are attached to bodies)
//Joint, (defined within bodies to create motion degrees of freedom); 4types: ball, slide, hinge, "free joint"
//DOF, degree of freedom are closely related to joints (joints=positioal information while DOFs=velocity and force information)
//Geom, 3D shapes attached to bodies (used for collision and rendering); mostly convex
//site, used for sensors, tendon routin, and slider-crank endpoints (are light geoms without collision and computation components)
//Camera, by default is freely moved by user but additional fixed cameras can be made
//light, can be fixed to world or moving bodies (uses OpenGL)

//Stand-alone elements: (described outside of kinematic tree and do not belong to individual body)
//reference pose, (initial model pose)
//spring reference pose, (resting length of tendon springs); spring forces are generated when the joint configuration deviates from this
//tendon, scalar length elements used for actuation, imposing limits and equality constraints
//actuator, 3 components: transmission, activation dynamics, force generation mechanism
//sensor, generate sensor data saved in global array mjData.sensordata
//Equality, additional constraints beyond those already imposed by kinematic tree structure
//contact pair, contact generation and fine-tuning the contact model
//contact exclude, pairss of bodies which should be excluded from the generation of candidate contact pairs
//custom numeric, custom numbers (i.e. mjData.userdata is not used by any MuJoCo computations, the user can store results from custom compute)
//custom text, can be used in custom computations to specify keyword commands
//custom tuple, for specifying groups of elements that are needed for user code (not used in simulator)
//keyframe, snapshot of the simulation state

//Clarifications:
//Divergence, when elements of the state tend quickly to infinity (manifested in mjWARN_BADQACC) usually hints that timestep is too large
//Units are undefined, userbased defined units however the author suggests using MKS since the dafault value of gravity is (0,0,-9.81) and geom density 1000 which corresponds to density of water
//suprising collisions, by default MuJoCo excludes collisions between geoms that belong to body pairs which have a direct parent-child relationship (ex: forearm and elbow)
//not object-oriented, there are type-groupings such as mjModel and mjData but they should be passed through functions which expects them as pointers

//Bodies, Geoms and Sites: all roughly correspond to rigid bodies in the physical world. They were made separate because:
//geoms don't affect physics unlike bodies, hence geoms are mostly used for appearance and collision geometry
//sites are light geoms which mean they have the same appearance but do not participate in collisions or used to infer body masses.
//On the other hand, sites can do things geoms cannot such as specify the volume of touch sensor, end-point of slider-crank actuators...

// this short summary is that of MuJoCo's overview: https://mujoco.readthedocs.io/en/latest/overview.html
// basic.cc is a really good sample code provided by Mujoco

#include <mujoco/mujoco.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <cstring>


mjModel* m = NULL;
mjData* d = NULL;
mjvCamera cam;
mjvOption opt;
mjvScene scn;
mjrContext con;

bool button_left = false;
bool button_middle = false;
bool button_right = false;
double lastx = 0;
double lasty = 0;

void keyboard(GLFWwindow* window, int key, int scancode, int act, int mods)
{
	if (act == GLFW_PRESS && key == GLFW_KEY_BACKSPACE) {

		mj_resetData(m, d);
		mj_forward(m, d);
	}
}

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

void mouse_button(GLFWwindow* window, int button, int act, int mods) {

	button_left = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
	button_middle = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
	button_right = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

	glfwGetCursorPos(window, &lastx, &lasty);
}

void mouse_move(GLFWwindow* window, double xpos, double ypos) {

	if (!button_left && !button_middle && !button_right) {
		return;
	}

	double dx = xpos - lastx;
	double dy = ypos - lasty;
	lastx = xpos;
	lasty = ypos;

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	bool mod_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
	mjtMouse action;
	if (button_right) {
		action = mod_shift ? mjMOUSE_MOVE_H : mjMOUSE_MOVE_V;
	}
	else if (button_left) {
		action = mod_shift ? mjMOUSE_ROTATE_H : mjMOUSE_ROTATE_V;
	}
	else {
		action = mjMOUSE_ZOOM;
	}

	mjv_moveCamera(m, action, dx / height, dy / height, &scn, &cam);
}

void scroll(GLFWwindow* window, double xoffset, double yoffset) {
	// emulate vertical mouse motion = 5% of window height
	mjv_moveCamera(m, mjMOUSE_ZOOM, 0, -0.05 * yoffset, &scn, &cam);
}

const char filename[] = "C:/Users/ianmi/Documents/MuJoCo/model/humanoid/humanoid.xml";

int main() {
	
	char error[1000] = "Could not load model";
	m = mj_loadXML(filename, 0, error, 1000);

	// make data
	d = mj_makeData(m);

	//init GLFW
	if (!glfwInit()) {
		mju_error("Could not initialize GLFW");
	}

	//create window

	GLFWwindow* window = glfwCreateWindow(1200, 900, "Demo", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//init visualization data structures
	mjv_defaultCamera(&cam);
	mjv_defaultOption(&opt);
	mjv_defaultScene(&scn);
	mjr_defaultContext(&con);

	//create scene and context
	mjv_makeScene(m, &scn, 2000);
	mjr_makeContext(m, &con, mjFONTSCALE_150);

	//INSTALL glfw mouse and keyboard callbacks
	glfwSetKeyCallback(window, keyboard);
	glfwSetCursorPosCallback(window, mouse_move);
	glfwSetMouseButtonCallback(window, mouse_button);
	glfwSetScrollCallback(window, scroll);

	// run main loop, and simulation
	while (!glfwWindowShouldClose(window)) {

		mjtNum simstart = d->time;
		while (d->time - simstart < 1.0 / 60.0) {
			mj_step(m, d);
		}
		
		mjrRect viewport = { 0, 0, 0, 0 };
		glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

		mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);
		mjr_render(viewport, &scn, &con);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	mjv_freeScene(&scn);
	mjr_freeContext(&con);

	mj_deleteData(d);
	mj_deleteModel(m);

	return 1;
}

