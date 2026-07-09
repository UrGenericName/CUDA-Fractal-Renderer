#pragma once
#include "FractalRenderer.h"

void renderMandelbrotSetCUDA(char* buffer, int width, int height, float scale, float posX, float posY) {

    for (int i = 0; i < width * height; ++i) {

        int x = i % width;
        int y = i / width;

        size_t pixelStartIndex = 3 * (y * width + x);

        float x_normalized = (static_cast<float>(x) / static_cast<float>(width));
        float y_normalized = (static_cast<float>(y) / static_cast<float>(height));

        // Slightly increases the size of either x or y to account for non-square aspect ratio
        if (width > height) {
            float ratio = (static_cast<float>(width) / height);
            x_normalized *= ratio;
            x_normalized -= (ratio - 1.0f) / 2.0f;
        }
        else {
            float ratio = (static_cast<float>(height) / width);
            y_normalized *= ratio;
            y_normalized -= (ratio - 1.0f) / 2.0f;
        }

        float x_global = (x_normalized - 0.5f) * scale + 0.5f + posX;
        float y_global = (y_normalized - 0.5f) * scale + 0.5f + posY;

        float z_real = 0.0f;
        float z_imag = 0.0f;
        float c_real = x_global;
        float c_imag = y_global;

        int iteration = 0;
        while (iteration <= 50) {
            // Cache the squares to avoid calculating them multiple times
            float r2 = z_real * z_real;
            float i2 = z_imag * z_imag;

            // Check magnitude: |z|^2 = real^2 + imag^2
            // If it exceeds 4 (radius of 2), it escapes
            if (r2 + i2 > 4.0f) {
                break;
            }

            // Calculate next z: z = z^2 + c
            // New Real = real^2 - imag^2 + c_real
            // New Imag = 2 * real * imag + c_imag
            float next_real = r2 - i2 + c_real;
            z_imag = 2.0f * z_real * z_imag + c_imag;
            z_real = next_real;

            iteration++;
        }

        buffer[pixelStartIndex + 0] = 0;
        buffer[pixelStartIndex + 1] = 0;
        buffer[pixelStartIndex + 2] = 0;

        if (iteration < 50) buffer[pixelStartIndex + 2] = 255;

    }

}

void renderJuliaSetCUDA(char* buffer, int width, int height, float scale, float posX, float posY) {}

void FractalRenderer::generateGPU(char* buffer) {

    switch (fractalType) {

    case FractalType::MANDELBROT:
        renderMandelbrotSetCUDA(buffer, width, height, scale, pos.x, pos.y);
        break;

    case FractalType::JULIA:
        renderJuliaSetCUDA(buffer, width, height, scale, pos.x, pos.y);
        break;

    }

}