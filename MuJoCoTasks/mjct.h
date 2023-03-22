#pragma once
#include <iostream>
#include <mujoco/mujoco.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
//The goal of this header file is to contain classes that can be implemented nicely to python bindings
//TODO: add condition to render function in tosser class

class FixedEnvironment {

public:
	mjModel* m = NULL;
	mjData* d = NULL;
	bool b_render = false;

	//these are optional variables
	mjvOption opt;
	mjvCamera cam;
	mjvScene scn;
	mjrContext con;
	mjrRect viewport;
	unsigned char* rgb = NULL;
	float* depth = NULL;
	int H;
	int W;
	GLFWwindow* window;

	void create(const char* taskname, bool render = false) {

		b_render = render;

		//necessary init
		char error[100] = "Could not load model";
		m = mj_loadXML(taskname, 0, error, 100);
		d = mj_makeData(m);

		if (render) {

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
			mjv_defaultOption(&opt); //these are visualization configs
			mjv_defaultCamera(&cam);
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
	}

	void setup_camera(double lookat[3], mjtNum azimuth, mjtNum elevation, mjtNum distance) {
		if (render) 
		{
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
		else 
		{
			//should be a Warning
			std::cout << "This entity is off-render!";
		}
	}

	unsigned char* render() {
		if (render) 
		{
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
		else
		{
			//should be a warning
			std::cout << "Attempting to render on off-render entity!";
		}
	}

	~FixedEnvironment() {

		// close file, free buffers and OpenGL
		if (render) 
		{
			std::free(rgb);
			std::free(depth);
			glfwTerminate();
			mjr_freeContext(&con);
			mjv_freeScene(&scn);
		}

		// close MuJoCo
		mj_deleteData(d);
		mj_deleteModel(m);
	}
};


//this is the class that should be binded to python
class Tosser {

	FixedEnvironment env;
	std::string info = "";
	
	//init
	Tosser(bool render)
	{
		env.create("tosser.xml", render); //add render argument

		//set camera position
		double pos[] = { 0.022, -0.678, 0.393 };
		env.setup_camera(pos, -173.2, -13.4, 1.8);
	}

	//step function
	std::tuple <std::array<double, 10>, double, bool, bool, std::string> step(double action[2])
	{
		//process action
		env.d->ctrl[0] = action[0];
		env.d->ctrl[1] = action[1];

		//step
		mj_step(env.m, env.d);


		//filter reward
		double reward;
		if (env.d->sensordata[2] > 9) {
			//red bucket
			reward = 0.5;
		}
		else if (env.d->sensordata[3] > 9) {
			//green bucket
			reward = 1.0;
		}
		else {
			reward = 0.0;
		}

		//process observation
		std::array<double, 10> observation;
		for (int i = 0; i < 5; i++)
		{
			observation[i+5] = env.d->qpos[i];
			observation[i] = env.d->qvel[i];
		}
		
		//process termination
		bool done;
		if (env.d->time > 5 || reward != 0.0)
		{
			done = true;
		}
		else {
			done = false;
		}

		//truncated
		// 
		//Nothing for now...

		//create output
		std::tuple <std::array<double, 10> , double, bool, bool, std::string> output;
		output = std::make_tuple(observation, reward, done, false, info);

		return output;
	}


	//reset function
	std::tuple <std::array<double, 10>, std::string> reset()
	{
		//reset env and step forward
		mj_resetData(env.m, env.d);
		mj_step(env.m, env.d);

		//process observation
		std::array<double, 10> observation;
		for (int i = 0; i < 5; i++)
		{
			observation[i+5] = env.d->qpos[i];
			observation[i] = env.d->qvel[i];
		}

		//create output
		std::tuple <std::array<double, 10>, std::string> output;
		output = std::make_tuple(observation, info);

		return output;
	}

	//render function
	unsigned char* render()
	{
		return env.render();
	}
};
