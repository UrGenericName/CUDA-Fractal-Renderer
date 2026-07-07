#pragma once
#include <vector>
#include "Color.h"

class FractalRenderer {
public:

	double x_max = 1.0f;
	double x_min = -1.0f;

	double y_max = 1.0f;
	double y_min = -1.0f;
	
	FractalRenderer(int i_width, int i_height) : width(i_width), height(i_height) {};
	void generate(std::vector<Color>& buffer);

private:
	int width;
	int height;
};