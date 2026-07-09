#pragma once
#include "FractalRenderer.h"

#include <thread>

using namespace std;

void FractalRenderer::generate(char* buffer) {

    switch (renderMethod) {

    case RenderMethod::CPU:
        generateCPU(buffer);
        break;

    case RenderMethod::CPU_MULTI_THREADED:
        generateMultiThreadedCPU(buffer);
        break;

    case RenderMethod::GPU:
        generateGPU(buffer);
        break;

    }
}

void FractalRenderer::generate() {
    generate(this->buffer);
}

void FractalRenderer::generateCPU(char* buffer) {

    renderPixels(buffer, 0, width * height);

}

void FractalRenderer::generateMultiThreadedCPU(char* buffer) {

    vector<thread> threads;
    unsigned int threadCount = thread::hardware_concurrency();
    int pixelsPerThread = (width * height) / threadCount;

    // Calculate a group of pixel for each thread
    for (int i = 0; i < threadCount; i++) {
        
        int offset = i * pixelsPerThread;

        // for thread i, render a specific group of pixels
        threads.push_back(std::thread([&buffer, offset, pixelsPerThread, this]() {

            renderPixels(buffer, offset, pixelsPerThread);

        }));

    }

    // Wait for all them to join back
    for (auto& t : threads) {
        t.join();
    }

}

void FractalRenderer::renderPixels(char* buffer, unsigned int offset, unsigned int pixelCount) {

    switch (fractalType) {

    case FractalType::MANDELBROT:
        renderMandelbrotSet(buffer, offset, pixelCount);
        break;

    case FractalType::JULIA:
        renderJuliaSet(buffer, offset, pixelCount);
        break;

    }

}

void FractalRenderer::renderMandelbrotSet(char* buffer, unsigned int offset, unsigned int pixelCount) {

    for (int i = offset; i < offset + pixelCount; ++i) {

        int x = i % width;
        int y = i / width;

        size_t pixelStartIndex = 3 * (y * width + x);

        Vector2 globalCoords = screenCoordToGlobal(x, y);
        int iteration = mandelbrotSetMath(globalCoords.x, globalCoords.y);
        Color color = calculateColor(iteration);


        buffer[pixelStartIndex + 0] = color.r;
        buffer[pixelStartIndex + 1] = color.g;
        buffer[pixelStartIndex + 2] = color.b;
    }

}

void FractalRenderer::renderJuliaSet(char* buffer, unsigned int offset, unsigned int pixelCount) {

    for (int i = offset; i < offset + pixelCount; ++i) {

        int x = i % width;
        int y = i / width;
        size_t pixelStartIndex = 3 * (y * width + x);

        Vector2 globalCoords = screenCoordToGlobal(x, y);
        int iteration = juliaSetMath(globalCoords.x, globalCoords.y);
        Color color = calculateColor(iteration);


        buffer[pixelStartIndex + 0] = color.r;
        buffer[pixelStartIndex + 1] = color.g;
        buffer[pixelStartIndex + 2] = color.b;;
    }

}

Vector2 FractalRenderer::screenCoordToGlobal(float x, float y) {

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

    float x_global = (x_normalized - 0.5f) * scale + 0.5f + pos.x;
    float y_global = (y_normalized - 0.5f) * scale + 0.5f + pos.y;

    return Vector2{ x_global, y_global };

}

Vector2 FractalRenderer::globalCoordToScreen(float x, float y) {

    // Reverse the position offset and scale scaling
    float x_normalized = (x - pos.x - 0.5f) / scale + 0.5f;
    float y_normalized = (y - pos.y - 0.5f) / scale + 0.5f;

    // Reverse the aspect ratio centering adjustments
    if (width > height) {
        float ratio = (static_cast<float>(width) / height);
        x_normalized += (ratio - 1.0f) / 2.0f;
        x_normalized /= ratio;
    }
    else {
        float ratio = (static_cast<float>(height) / width);
        y_normalized += (ratio - 1.0f) / 2.0f;
        y_normalized /= ratio;
    }

    // Reverse the normalization to map back to pixel dimensions
    float x_screen = x_normalized * static_cast<float>(width);
    float y_screen = y_normalized * static_cast<float>(height);

    return Vector2{ x_screen, y_screen };

}

inline int FractalRenderer::mandelbrotSetMath(float x, float y) {

    complex<float> z{ 0.0f, 0.0f };
    complex<float> c{ x, y };

    int iteration = 0;
    while (iteration <= maxIterations) {

        z = (z * z) + c;

        if (real(z) * real(z) + imag(z) * imag(z) > 4) break;
        iteration++;
    }

    return iteration;

}

inline int FractalRenderer::juliaSetMath(float x, float y) {

    complex<float> z{ x, y };
    complex<float> c{ juliaC.x, juliaC.y };

    int iteration = 0;
    while (iteration <= maxIterations) {

        z = (z * z) + c;

        if (real(z) * real(z) + imag(z) * imag(z) > 4) break;
        iteration++;
    }

    return iteration;

}

inline Color FractalRenderer::calculateColor(int iteration) {

    Color color = { 0, 0, 0, 255 };
    if (iteration <= maxIterations) {

        float hue = ((float)iteration / (float)maxIterations);
        unsigned char brightness = static_cast<unsigned char>(255.0f * hue);

        color = { 0, 0, brightness, 255 };

        Vector3 getHue = ColorToHSV(color);
        color = ColorFromHSV(getHue.x + ((1.0f - hue) * 360.0f), getHue.y, -((getHue.z - 1.0f) * (getHue.z - 1.0f)) + 1);

    }

    return color;

}