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

	unsigned char* render() {
		// update abstract scene
		mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);

		// render scene in offscreen buffer
		mjr_render(viewport, &scn, &con);

		// read rgb and depth buffers
		mjr_readPixels(rgb, depth, viewport, &con);

		// create OpenCV Mat object from rgb buffer.
		
		//cv::Mat rgb_mat(H, W, CV_8UC3, rgb);

		//cv::flip(rgb_mat, rgb_mat, 0);
		//cv::cvtColor(rgb_mat, rgb_mat, cv::COLOR_RGB2BGR);

		// save as JPEG
		//cv::imwrite("output.jpg", rgb_mat);
		return rgb;
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

#include <tuple>

//this is the class that should be binded to python
class Tosser {
	
	Tosser()
	{
		static FixedEnvironment env("tosser.xml");
	}

	



};