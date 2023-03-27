//The goal of this header file is to contain classes that can be implemented nicely to python bindings

#pragma once
#include <iostream>
#include <mujoco/mujoco.h>
#include <GLFW/glfw3.h>
#include <tuple>
#include <array>
#include <string>

//example of what components every task environment must have by default (in the documentation it is refered to as render-disabled or LightEnvironment)
//equivalent to render-disabled environment structure
/*struct LightEnvironment {

	mjModel* m;
	mjData* d;

	LightEnvironment(const char* taskname)
	{
		char error[100] = "Could not load model.xml";
		m = mj_loadXML(taskname, 0, error, 100);
		d = mj_makeData(m);
	}

	~LightEnvironment()
	{
		mj_deleteData(d);
		mj_deleteModel(m);
	}
};*/


//structure used for render-enabled environments
struct HeavyEnvironment {

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

	HeavyEnvironment(mjModel* m)
	{
		if (!glfwInit())
		{
			std::cout << "!glfwInit";
		}

		//make single buffer and invisible window
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, 0);
		window = glfwCreateWindow(800, 800, "MuJoCoTask", NULL, NULL);
		if (!window) {
			mju_error("Could not create GLFW window.");
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
			mju_error("Could not allocate buffers.");
		}
	}

	void setup_camera(double lookat[3], mjtNum azimuth, mjtNum elevation, mjtNum distance) {

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

	unsigned char* render(mjModel* m, mjData* d)
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

	~HeavyEnvironment() {

		// close file, free buffers and OpenGL
		std::free(rgb);
		std::free(depth);
		glfwTerminate();
		mjr_freeContext(&con);
		mjv_freeScene(&scn);
	}
};


const char* addToModel_Dir(const char* filename) {
	std::string current_file(__FILE__);
	std::size_t last_separator = current_file.find_last_of("/\\");
	std::string current_directory = current_file.substr(0, last_separator);
	return (current_directory + filename).c_str();
}

/*
mjModel loadModel(const char* pathfromconfigfolder) {
	mjModel m;
	char error[1000] = "Could not load tosser.xml";
	char* buf = nullptr;
	size_t sz = 0;
	if (_dupenv_s(&buf, &sz, "APPDATA") == 0 && buf != nullptr)
	{
		const char* filename = (std::string(buf) + "/mjct/tosser.xml").c_str();
		m = *mj_loadXML(filename, 0, error, 1000);
		free(buf);
		return m;
	}
	else
	{
		throw std::runtime_error(error);
		return m;
	}
}*/



//example of basic cpp environment
class TosserCPP {

private:
	mjModel* m;
	mjData* d;
	HeavyEnvironment* env;
	std::string info = "";
	bool b_render;
	bool terminated;

public:
	//init
	TosserCPP(bool render, double timestep)
	{
		terminated = false;
		// required init
		

		m->opt.timestep = timestep;
		d = mj_makeData(m);
		b_render = render;

		//optional init
		if (b_render){
			env = new HeavyEnvironment(m);
			//set camera position
			double pos[] = { 0.022, -0.678, 0.393 };
			env->setup_camera(pos, -173.2, -13.4, 1.8);
		}
		else {
			env = nullptr;
		}
	}

	//step function
	std::tuple <std::array<double, 10>, double, bool, bool, std::string> step(double action[2])
	{
		//process action
		d->ctrl[0] = action[0];
		d->ctrl[1] = action[1];

		//step
		mj_step(m, d);


		//filter reward
		double reward;
		if (d->sensordata[2] > 9) {
			//red bucket
			reward = 0.5;
		}
		else if (d->sensordata[3] > 9) {
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
			observation[i+5] = d->qpos[i];
			observation[i] = d->qvel[i];
		}
		
		//process termination
		bool done;
		if (d->time > 5 || reward != 0.0)
		{
			done = true;
		}
		else {
			done = false;
		}

		//truncated
		// 
		//Nothing for now...
		return std::make_tuple(observation, reward, done, false, info);
	}

	//reset function
	std::tuple <std::array<double, 10>, std::string> reset()
	{
		//reset env and step forward
		mj_resetData(m, d);
		mj_step(m, d);

		//process observation
		std::array<double, 10> observation;
		for (int i = 0; i < 5; i++)
		{
			observation[i+5] = d->qpos[i];
			observation[i] = d->qvel[i];
		}

		return std::make_tuple(observation, info);
	}

	//render function
	unsigned char* render()
	{
		if (b_render) {
			return env->render(m, d);
		}
		else {
			mju_error("LightEnvironment trying to render!");
		}
	}

	~TosserCPP()
	{
		mj_deleteData(d);
		mj_deleteModel(m);
		delete env;
	}
};