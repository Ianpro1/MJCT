#pragma once
#include <iostream>
#include <string>
#include <array>
#include <tuple>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <mujoco/mujoco.h>

namespace py = pybind11;




//example of basic task environment
class Tosser {

private:
	mjModel* m;
	mjData* d;
	std::string info = "";
	bool b_render;
	bool terminated;
public:
	//init
	Tosser(bool render = false)
	{	
		// required init
		char error[100] = "Could not load tosser.xml";
		m = mj_loadXML("tosser.xml", 0, error, 100);
		if (!m) {
			throw std::runtime_error(error);
		}

		d = mj_makeData(m);
		b_render = render;
	}

	//step function
	std::tuple <std::array<double, 10>, double, bool, bool, std::string> step(py::array_t<double,2> values)
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

	~Tosser()
	{
		mj_deleteData(d);
		mj_deleteModel(m);
	}
};


std::array<double, 2> arr(py::array_t<double, 1> value) {

	py::buffer_info info = value.request();
	if (info.ndim != 1 || info.shape[0] != 2) {
		throw std::runtime_error("Input must be an array of shape (2,)");
	}
	
	double* arr = static_cast<double*>(info.ptr);

	std::array<double, 2> obs;
	obs[0] = arr[1];
	obs[1] = arr[0];
	return obs;
}


