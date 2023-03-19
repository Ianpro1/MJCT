#pragma once
#include <iostream>
#include <mujoco/mujoco.h>


//the goal is to end up with a class that can be implemented nicely to python binding
class Environment {

public:
	mjModel *m;
	mjData* d;
	mjvCamera cam;
	mjvOption opt;
	mjvScene scn;
	mjrContext con;
	mjrRect viewport = { 0, 0, 1200, 900 };

	Environment(mjModel* model=NULL) {
		m = model;
		d = mj_makeData(m);
		mjv_defaultCamera(&cam);
		mjv_defaultOption(&opt);
		mjv_defaultScene(&scn);
		mjr_defaultContext(&con);
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
		mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);
		mjr_render(viewport, &scn, &con);
	}

	~Environment() {
		mjv_freeScene(&scn);
		mjr_freeContext(&con);
		mj_deleteData(d);
		mj_deleteModel(m);
	}

};