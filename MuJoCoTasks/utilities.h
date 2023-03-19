#pragma once
#include <mujoco/mujoco.h>
#include <iostream>

void print_namedvect(const char name[], const int size, const double* list) {
	std::cout << name;
	std::cout << " [";
	for (int i = 0; i < size; i++) {
		std::cout << list[i];
		if (i != size - 1)
		{
			std::cout << ", ";
		}
	}
	std::cout << "]\n";
}

void print_namedscalar(const char name[], const double scalar) {
	std::cout << name;
	std::cout << " [";
	std::cout << scalar;
	std::cout << "]\n";
}

void print_cam(const mjvCamera cam) {
	print_namedvect("lookat", 3, cam.lookat);
	print_namedscalar("azimuth", cam.azimuth);
	print_namedscalar("elevation", cam.elevation);
	print_namedscalar("distance", cam.distance);
	std::cout << "\n";
}

