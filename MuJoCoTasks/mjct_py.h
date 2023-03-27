#pragma once
#include <iostream>
#include <string>
#include <array>
#include <tuple>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <mujoco/mujoco.h>
#include  <GLFW/glfw3.h>
#include <mjct.h>

namespace py = pybind11;

//example of basic task environment (this is where python bindings are implemented)
class Tosser {

private:
	mjModel* m;
	mjData* d;
	HeavyEnvironment* env;
	std::string info = "";
	bool b_render;
	bool terminated;
	
public:
	//init
	Tosser(const char* path, bool render, double timestep)
	{	// required init
		terminated = false;

		char error[1000] = "Could not load tosser.xml";
		m = mj_loadXML(path, 0, error, 1000);
		if(!m){
			throw std::runtime_error(error);
		}

		m->opt.timestep = timestep;
		d = mj_makeData(m);
		b_render = render;
		//optional init
		if (b_render) {
			env = new HeavyEnvironment(m);
			//set camera position
			double pos[] = { 0.022, -0.678, 0.393 };
			env->setup_camera(pos, -173.2, -13.4, 2.8);
		}
		else {
			env = nullptr;
		}
	}

	//step function
	std::tuple<std::array<double,10>, double, bool, bool, std::string> step(py::array_t<double, py::array::c_style | py::array::forcecast> values)
	{
		py::buffer_info buffer_info = values.request();
		if (buffer_info.ndim != 1 || buffer_info.shape[0] != 2) {
			throw std::runtime_error("Input must be an array of shape (2,)");
		}

		double* action = static_cast<double*>(buffer_info.ptr);

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
			observation[i + 5] = d->qpos[i];
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


		if (terminated) {
			std::cout << "The environment is already terminated! further steps may lead to innacurate environment observations.";
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
			observation[i + 5] = d->qpos[i];
			observation[i] = d->qvel[i];
		}

		return std::make_tuple(observation, info);
	}

	//render function
	py::array_t<unsigned char, py::array::c_style | py::array::forcecast> render() {

		// assumes an image of shape 800 * 800 * 3
		unsigned char* rgb = env->render(m, d);

		// create Numpy array
		py::array_t<unsigned char, py::array::c_style | py::array::forcecast> pixels({ 800, 800, 3 }, { 800 * 3, 3, 1 }, rgb);

		// reverse the order of the rows
		unsigned char* pixel_buffer = pixels.mutable_data();
		for (int i = 0; i < 800 / 2; i++) {
			std::swap_ranges(pixel_buffer + i * 800 * 3, pixel_buffer + (i + 1) * 800 * 3, pixel_buffer + (800 - i - 1) * 800 * 3);
		}

		return pixels;
	}


	~Tosser()
	{
		mj_deleteData(d);
		mj_deleteModel(m);
		delete env;
	}
};
