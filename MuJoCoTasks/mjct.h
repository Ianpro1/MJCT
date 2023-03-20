#pragma once
#include <iostream>
#include <mujoco/mujoco.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>

//The goal of this header file is to contain classes that can be implemented nicely to python bindings

class FixedEnvironment {

public:
	mjModel* m = NULL;
	mjData* d = NULL;
	mjvCamera cam;
	mjvOption opt;
	mjvScene scn;
	mjrContext con;
	mjrRect viewport;
	unsigned char* rgb = NULL;
	float* depth = NULL;
	int H;
	int W;
	GLFWwindow* window;

	FixedEnvironment(const char* taskname) {

		if (!glfwInit())
		{
			std::cout << "!glfwInit";
		}
		//make single buffer and invisible window
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, 0);
		window = glfwCreateWindow(800, 800, "MuJoCoTask", NULL, NULL);
		if (!window) {
			mju_error("Could not create GLFW window");
		}

		// make context current
		glfwMakeContextCurrent(window);
		char error[100] = "Could not load model";
		m = mj_loadXML(taskname, 0, error, 100);
		d = mj_makeData(m);
		mjv_defaultCamera(&cam);
		mjv_defaultOption(&opt);
		mjv_defaultScene(&scn);
		mjr_defaultContext(&con);

		//user defined settings
		mjv_makeScene(m, &scn, 1000);
		mjr_makeContext(m, &con, 200);

		viewport = mjr_maxViewport(&con);
		W = viewport.width;
		H = viewport.height;

		rgb = (unsigned char*)std::malloc(3 * W * H);
		depth = (float*)std::malloc(sizeof(float) * W * H);
		if (!rgb || !depth) {
			mju_error("Could not allocate buffers");
		}
	}

	// deallocate everything
	void closeMuJoCo(void)
	{
		mj_deleteData(d);
		mj_deleteModel(m);
		mjr_freeContext(&con);
		mjv_freeScene(&scn);
	}

	void closeOpenGL()
	{
		glfwTerminate();
	}

	void setup_camera(mjtNum lookat[3], mjtNum azimuth, mjtNum elevation, mjtNum distance) {
		//camera position
		for (int i = 0; i < 3; i++)
		{
			cam.lookat[i] = lookat[i];
		}
		//camera angle
		cam.azimuth = azimuth;
		cam.elevation = elevation;
		//camera distance
		cam.distance = distance;
	}

	void reset() {
		mj_resetData(m, d);
	}

	void step() {
		mj_step(m, d);
	}

	void render() {
		// update abstract scene
		mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);

		// render scene in offscreen buffer
		mjr_render(viewport, &scn, &con);

		// read rgb and depth buffers
		mjr_readPixels(rgb, depth, viewport, &con);

		// create OpenCV Mat object from rgb buffer
		cv::Mat rgb_mat(H, W, CV_8UC3, rgb);

		cv::flip(rgb_mat, rgb_mat, 0);
		cv::cvtColor(rgb_mat, rgb_mat, cv::COLOR_RGB2BGR);

		// save as JPEG
		cv::imwrite("output.jpg", rgb_mat);
	}

	~FixedEnvironment() {
		// close file, free buffers
		std::free(rgb);
		std::free(depth);

		// close MuJoCo and OpenGL
		closeMuJoCo();
		closeOpenGL();
	}

};




//Attempt to create Environment class with mouse and keyboard callbacks

/*
class FreeEnvironment
{
public:
	static mjModel* m;
	static mjData* d;
	static mjvCamera cam;
	mjvOption opt;
	static mjvScene scn;
	mjrContext con;
	GLFWwindow* window;

	//keyboard mouse variables
	static bool button_left;
	static bool button_middle;
	static bool button_right;
	static double lastx;
	static double lasty;

	static void keyboard(GLFWwindow* window, int key, int scancode, int act, int mods)
	{
		if (act == GLFW_PRESS && key == GLFW_KEY_BACKSPACE) {

			mj_resetData(m, d);
			mj_forward(m, d);
		}
	}

	static void mouse_button(GLFWwindow* window, int button, int act, int mods) {

		button_left = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
		button_middle = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
		button_right = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

		glfwGetCursorPos(window, &lastx, &lasty);
	}

	static void mouse_move(GLFWwindow* window, double xpos, double ypos) {

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

	static void scroll(GLFWwindow* window, double xoffset, double yoffset) {
		// emulate vertical mouse motion = 5% of window height
		mjv_moveCamera(m, mjMOUSE_ZOOM, 0, -0.05 * yoffset, &scn, &cam);
	}

	FreeEnvironment(const char* taskname)
	{
		if (!glfwInit())
		{
			std::cout << "Couldn't initialize opengl";
		}
		//make visible window
		window = glfwCreateWindow(1200, 900, "MuJoCoTask", NULL, NULL);
		if (!window) {
			mju_error("Could not create GLFW window");
		}

		// make context current
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		char error[100] = "Could not load model";
		m = mj_loadXML(taskname, 0, error, 100);
		d = mj_makeData(m);
		mjv_defaultCamera(&cam);
		mjv_defaultOption(&opt);
		mjv_defaultScene(&scn);
		mjr_defaultContext(&con);

		//user defined settings
		mjv_makeScene(m, &scn, 1000);
		mjr_makeContext(m, &con, 200);

		button_left = false;
		button_middle = false;
		button_right = false;
		lastx = 0;
		lasty = 0;

		//INSTALL glfw mouse and keyboard callbacks
		glfwSetKeyCallback(window, keyboard);
		glfwSetCursorPosCallback(window, mouse_move);
		glfwSetMouseButtonCallback(window, mouse_button);
		glfwSetScrollCallback(window, scroll);
	}


	// deallocate everything
	void closeMuJoCo(void)
	{
		mj_deleteData(d);
		mj_deleteModel(m);
		mjr_freeContext(&con);
		mjv_freeScene(&scn);
	}

	void closeOpenGL()
	{
		glfwTerminate();
	}

	void setup_camera(mjtNum lookat[3], mjtNum azimuth, mjtNum elevation, mjtNum distance) {
		//camera position
		for (int i = 0; i < 3; i++)
		{
			cam.lookat[i] = lookat[i];
		}
		//camera angle
		cam.azimuth = azimuth;
		cam.elevation = elevation;
		//camera distance
		cam.distance = distance;
	}

	void reset() {
		mj_resetData(m, d);
	}

	void step() {
		mj_step(m, d);
	}

	void render_step() {
		//set framebuffer size
		mjrRect viewport = { 0, 0, 0, 0 };
		glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

		//render
		mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);
		mjr_render(viewport, &scn, &con);
		
		//swap buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	~FreeEnvironment() {
		// close MuJoCo and OpenGL
		closeMuJoCo();
		closeOpenGL();
	}
};*/