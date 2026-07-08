#pragma once
#include "FractalRenderer.h"

FractalRenderer::FractalRenderer(int i_width, int i_height) : width(i_width), height(i_height) {
    buffer.resize(width * height);
}

void FractalRenderer::generate(std::vector<Color>& buffer) {

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            float x_normalized = (static_cast<float>(x) / static_cast<float>(width));
            float y_normalized = (static_cast<float>(y) / static_cast<float>(height));

            if (width > height) {
                x_normalized *= (static_cast<float>(width) / height);
            } else {
                y_normalized *= (static_cast<float>(height) / width);
            }

            float cx = (x_normalized - 0.5f) * scale + 0.5f + pos.x;
            float cy = (y_normalized - 0.5f) * scale + 0.5f + pos.y;

            float zx = 0.0f;
            float zy = 0.0f;
            int iteration = 0;

            while ((zx * zx + zy * zy) <= 4.0f && iteration < maxIterations) {
                float next_zx = (zx * zx) - (zy * zy) + cx;
                float next_zy = (2.0f * zx * zy) + cy;
                zx = next_zx;
                zy = next_zy;
                iteration++;
            }

            Color color = { 0, 0, 0, 255 };
            if (iteration <= maxIterations) color = { 0, static_cast<unsigned char>(255 * ((float)iteration / (float)maxIterations)), static_cast<unsigned char>(255 * ((float)iteration / (float)maxIterations)), 255};

            buffer[y * width + x] = color;

        }
    }

}

void FractalRenderer::generate() {
    generate(this->buffer);
}