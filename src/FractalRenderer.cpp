#pragma once
#include "FractalRenderer.h"

void FractalRenderer::generate(std::vector<Color>& buffer) {

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            float x_normalized = (static_cast<float>(x) / static_cast<float>(width));
            float y_normalized = (static_cast<float>(y) / static_cast<float>(height));

            float cx = (x_normalized * (x_max - x_min)) + x_min;
            float cy = (y_normalized * (y_max - y_min)) + y_min;

            float zx = 0.0f;
            float zy = 0.0f;
            int iteration = 0;

            while ((zx * zx + zy * zy) <= 4.0f && iteration < MAX_ITERATIONS) {
                float next_zx = (zx * zx) - (zy * zy) + cx;
                float next_zy = (2.0f * zx * zy) + cy;
                zx = next_zx;
                zy = next_zy;
                iteration++;
            }



            Color color = { 0, 0, 0 };
            if (iteration <= MAX_ITERATIONS) color = { static_cast<unsigned char>(255 * ((float)iteration / (float)MAX_ITERATIONS)), 0, 0};

            buffer[y * width + x] = color;

        }
    }

}