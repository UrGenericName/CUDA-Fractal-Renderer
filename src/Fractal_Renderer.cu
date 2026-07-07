#pragma once
#include "Fractal_Renderer.cuh"

void generateFractalGPU(std::vector<Color>& buffer, int width, int height) {

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {

			Color color = { static_cast<int>(255 * (float(x) / float(width))), static_cast<int>(255 * (float(y) / float(height))), 255 };

			buffer[y * width + x] = calculateFractalPixel( (float(x) / float(width)), (float(y) / float(height)) );

		}
	}

}

Color calculateFractalPixel(double x, double y) {

    x = 2.0f * x - 1.0f;
    y = 2.0f * y - 1.0f;

    float cx = x;
    float cy = y;
    float zx = 0.0f;
    float zy = 0.0f;
    int iteration = 0;
    const int MAX_ITERATIONS = 20;

    while ((zx * zx + zy * zy) <= 4.0f && iteration < MAX_ITERATIONS) {
        float next_zx = (zx * zx) - (zy * zy) + cx;
        float next_zy = (2.0f * zx * zy) + cy;
        zx = next_zx;
        zy = next_zy;
        iteration++;
    }

    Color color = { 0, 0, 0 };
    if (iteration == 20) color = { 255, 0, 0 };

	return color;

}