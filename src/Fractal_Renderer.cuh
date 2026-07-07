#pragma once
#include <vector>
#include "Color.h"

void generateFractalGPU(std::vector<Color>& buffer, int width, int height);
Color calculateFractalPixel(double x, double y);