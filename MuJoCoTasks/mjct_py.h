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

//structure used for render-enabled environments (Python approach)
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
		// 
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


//example of basic environment (using Python bindings pre-implemented)
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
	Tosser(const char* path, bool render, double timestep, double apirate)
	{	// required init
		terminated = false;

		char error[1000] = "Could not load tosser.xml";
		m = mj_loadXML(path, 0, error, 1000);
		if(!m){
			mju_error(error);
		}

		m->opt.timestep = timestep;
		m->opt.apirate = apirate;
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
			observation[i + 5] = d->qpos[i];
			observation[i] = d->qvel[i];
		}

		return std::make_tuple(observation, info);
	}

	//render function
	py::array_t<unsigned char, py::array::c_style | py::array::forcecast> render() {
		if (b_render) {
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
		else {
			const char* error = "Cannot render from render-disabled environment!\n";
			//test mju_error to python
			mju_error(error);
			//throw std::runtime_error(error);
		}
	}


	~Tosser()
	{
		mj_deleteData(d);
		mj_deleteModel(m);
		delete env;
	}
};
