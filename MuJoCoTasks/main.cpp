#include <mujoco/mujoco.h>
#include <GLFW/glfw3.h>
#include <iostream>

//Documentation Summary

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
//sites are light geoms which mean they have the same appearance but do not participate in collsions or used to infer body masses.
//On the other hand, sites can do things geoms cannot such as specify the volume of touch sensor, end-point of slider-crank actuators...

// this short summary is that of MuJoCo's overview: https://mujoco.readthedocs.io/en/latest/overview.html

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

int contacts[1];



void simulate(int id, int nsteps, mjtNum* ctrl)
{
	for (int i = 0; i < nsteps; i++) 
	{

	
	}

}


int main() {

	char error[1000] = "Could not load binary model";
	mjModel* m = NULL;
	// model can also be loaded in binary format
	m = mj_loadXML("C:/Users/ianmi/Documents/MuJoCo/model/humanoid/humanoid.xml", 0, error, 1000);
	if (!m)
	{
		return finish(error);
	}




	return finish("done", m);
}






