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
//Also, don't forget to implement a getState and setState function to make the class pickable
/*struct LightEnvironment {

	mjModel* m;
	mjData* d;

	LightEnvironment(const char* taskNameorPath)
	{
		char error[1000] = "Could not load model.xml";
		m = mj_loadXML(taskNameorPath, 0, error, 1000);
		if(!m){
		throw std::runtime_error(error);
		}
		d = mj_makeData(m);
	}

	~LightEnvironment()
	{
		mj_deleteData(d);
		mj_deleteModel(m);
	}
};*/


//this is a C-like approach to render environments
struct CPPEnvironment {

	mjvOption opt;
	mjvCamera cam;
	mjvScene scn;
	mjrContext con;
	GLFWwindow* window;
	bool not_closed_window;

	CPPEnvironment(mjModel* m)
	{
		not_closed_window = true;
		if (!glfwInit())
		{
			std::cout << "!glfwInit";
		}

		//make single buffer and invisible window
		window = glfwCreateWindow(800, 800, "MuJoCoTask", NULL, NULL);
		if (!window) {
			mju_error("Could not create GLFW window.");
		}
		// make context current
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		mjv_defaultOption(&opt); //these are visualization configs
		mjv_defaultCamera(&cam);
		mjv_defaultScene(&scn);
		mjr_defaultContext(&con);

		//user defined settings
		mjv_makeScene(m, &scn, 1000);
		mjr_makeContext(m, &con, 200);
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

	void render(mjModel* m, mjData* d)
	{
		//created viewport
		if (glfwWindowShouldClose(window) && not_closed_window) {
			not_closed_window = false;
			glfwTerminate();
		}
		else if(not_closed_window) 
		{
			mjrRect viewport = { 0, 0, 0, 0 };
			glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

			// update abstract scene
			mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);

			// render scene in offscreen buffer
			mjr_render(viewport, &scn, &con);

			//swap buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	~CPPEnvironment() {

		// close file, free buffers and OpenGL
		if (not_closed_window) {
			glfwTerminate();
		}
		mjr_freeContext(&con);
		mjv_freeScene(&scn);
	}
};


//example of basic cpp environment
class TosserCPP {

private:
	mjModel* m;
	mjData* d;
	CPPEnvironment* env;
	std::string info = "";
	bool terminated;
public:
	//parameters for pickling support or deep copy (if ever needed)
	const char* _path;
	bool b_render;
	double _params[2];

	//init
	TosserCPP(const char* path, bool render, double timestep, double apirate)
	{
		terminated = false;
		// required init

		char error[1000] = "Could not load tosser.xml";
		m = mj_loadXML(path, 0, error, 1000);
		if (!m) {
			mju_error(error);
		}
		else {
			//setting up custom options
			m->opt.timestep = timestep;
			m->opt.apirate = apirate;

			//saving parameters
			_params[0] = timestep;
			_params[1] = apirate;
		}

		d = mj_makeData(m);
		b_render = render;

		//optional init
		if (b_render){
			env = new CPPEnvironment(m);
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
		if (d->time > 4 || d->qpos[2] < -0.94 && d->qpos[3] < -0.30 || reward != 0.0)
		{
			done = true;
		}
		else {
			done = false;
		}

		//truncated
		// 
		//Nothing for now...
		if (terminated) {
			std::cout << "The environment is already terminated! further steps may lead to innacurate environment observations\n";
		}
		else if (done) {
			terminated = true;
		}
		return std::make_tuple(observation, reward, done, false, info);
	}

	//reset function
	std::tuple <std::array<double, 10>, std::string> reset()
	{
		terminated = false;
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
	void render()
	{
		if (b_render) {
			env->render(m, d);
		}
		else {
			const char* error = "Cannot render from render-disabled environment!\n";
			//test mju_error to python
			mju_error(error);
			//throw std::runtime_error(error);
		}
	}
	
	~TosserCPP()
	{
		mj_deleteData(d);
		mj_deleteModel(m);
		delete env;
	}

};
