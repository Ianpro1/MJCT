#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mujoco/mujoco.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>

mjModel* m;
mjData* d;

mjrContext con;
mjvScene scn;
mjvCamera cam;
mjvOption opt;
const char taskname[] = "tosser.xml";
const char outputname[] = "rbg.out";

void mujocoInit()
{
	char error[100] = "Could not load model";
	m = mj_loadXML(taskname, 0, error, 100);
	d = mj_makeData(m);

	mjr_defaultContext(&con);
	mjv_defaultCamera(&cam);
	mjv_defaultOption(&opt);
	mjv_defaultScene(&scn);

	mjv_makeScene(m, &scn, 1000);
	mjr_makeContext(m, &con, 200);

	//camera position
	cam.lookat[0] = 0.022;
	cam.lookat[1] = -0.878;
	cam.lookat[2] = 0.293;
	//camera angle
	cam.azimuth = -173.2;
	cam.elevation = -13.4;
	//camera distance
	cam.distance = 2.493;
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

void openglInit()
{
	//make single buffer and invisible window
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
	glfwWindowHint(GLFW_VISIBLE, 0);
	GLFWwindow* window = glfwCreateWindow(800, 800, "MuJoCoTask", NULL, NULL);
	if (!window) {
		mju_error("Could not create GLFW window");
	}

	// make context current
	glfwMakeContextCurrent(window);
}
void close_opengl()
{
	glfwTerminate();

}

int testmain()
{
	if (!glfwInit())
	{
		std::cout << "!glfwInit";
		return -1;
	}
	openglInit();
	mujocoInit();


	mjr_setBuffer(mjFB_OFFSCREEN, &con);
	if (con.currentBuffer != mjFB_OFFSCREEN) {
		std::printf("Warning: offscreen rendering not supported, using default/window framebuffer\n");
	}

	// get size of active renderbuffer
	mjrRect viewport = mjr_maxViewport(&con);
	int W = viewport.width;
	int H = viewport.height;

	// allocate rgb and depth buffers
	unsigned char* rgb = (unsigned char*)std::malloc(3 * W * H);
	float* depth = (float*)std::malloc(sizeof(float) * W * H);
	if (!rgb || !depth) {
		mju_error("Could not allocate buffers");
	}

	for (int i = 0; i < 10; i++)
	{
		mj_step(m, d);
	}

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

	// close file, free buffers
	std::free(rgb);
	std::free(depth);

	// close MuJoCo and OpenGL
	closeMuJoCo();
	closeOpenGL();

	return 1;
}