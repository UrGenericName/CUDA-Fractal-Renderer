#pragma once
#include "FractalRenderer.h"

#include <filesystem>
#include <chrono>
#include <thread>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace fs = std::filesystem;
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

        Real globalX, globalY;
        screenCoordToGlobal(x, y, &globalX, &globalY);
        int iteration = mandelbrotSetMath(maxIterations, globalX, globalY);
        Color color = calculateColor(maxIterations, iteration);


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

        Real globalX, globalY;
        screenCoordToGlobal(x, y, &globalX, &globalY);
        int iteration = juliaSetMath(maxIterations, globalX, globalY, juliaCx, juliaCy);
        Color color = calculateColor(maxIterations, iteration);


        buffer[pixelStartIndex + 0] = color.r;
        buffer[pixelStartIndex + 1] = color.g;
        buffer[pixelStartIndex + 2] = color.b;;
    }

}

void FractalRenderer::screenCoordToGlobal(Real x, Real y, Real* x_global, Real* y_global) {

    Real x_normalized = (static_cast<Real>(x) / static_cast<Real>(width));
    Real y_normalized = (static_cast<Real>(y) / static_cast<Real>(height));

    // Slightly increases the size of either x or y to account for non-square aspect ratio
    if (width > height) {
        Real ratio = (static_cast<Real>(width) / height);
        x_normalized *= ratio;
        x_normalized -= (ratio - 1.0f) / 2.0f;
    }
    else {
        Real ratio = (static_cast<Real>(height) / width);
        y_normalized *= ratio;
        y_normalized -= (ratio - 1.0f) / 2.0f;
    }

    *x_global = (x_normalized - 0.5f) * scale + 0.5f + posX;
    *y_global = (y_normalized - 0.5f) * scale + 0.5f + posY;

}

void FractalRenderer::globalCoordToScreen(Real x, Real y, Real* x_screen, Real* y_screen) {

    // Reverse the position offset and scale scaling
    Real x_normalized = (x - posX - 0.5f) / scale + 0.5f;
    Real y_normalized = (y - posY - 0.5f) / scale + 0.5f;

    // Reverse the aspect ratio centering adjustments
    if (width > height) {
        Real ratio = (static_cast<Real>(width) / height);
        x_normalized += (ratio - 1.0f) / 2.0f;
        x_normalized /= ratio;
    }
    else {
        Real ratio = (static_cast<Real>(height) / width);
        y_normalized += (ratio - 1.0f) / 2.0f;
        y_normalized /= ratio;
    }

    // Reverse the normalization to map back to pixel dimensions
    *x_screen = x_normalized * static_cast<Real>(width);
    *y_screen = y_normalized * static_cast<Real>(height);

}

inline int FractalRenderer::mandelbrotSetMath(int maxIterations, Real x, Real y) {

    Real zx = 0.0f;
    Real zy = 0.0f;

    Real cx = x;
    Real cy = y;

    int iteration = 0;
    while (iteration <= maxIterations) {

        Real zx2 = (zx * zx) - (zy * zy);
        Real zy2 = 2 * zx * zy;

        zx = zx2 + cx;
        zy = zy2 + cy;

        Real distanceFromOriginSquared = (zx * zx) + (zy * zy);

        if (distanceFromOriginSquared > 4) break;
        iteration++;
    }

    return iteration;

}

inline int FractalRenderer::juliaSetMath(int maxIterations, Real x, Real y, Real juliaCx, Real juliaCy) {

    Real zx = x;
    Real zy = y;

    Real cx = juliaCx;
    Real cy = juliaCy;

    int iteration = 0;
    while (iteration <= maxIterations) {

        Real zx2 = (zx * zx) - (zy * zy);
        Real zy2 = 2 * zx * zy;

        zx = zx2 + cx;
        zy = zy2 + cy;

        Real distanceFromOriginSquared = (zx * zx) + (zy * zy);

        if (distanceFromOriginSquared > 4) break;
        iteration++;
    }

    return iteration;


}

inline Color FractalRenderer::calculateColor(int maxIterations, int iteration) {

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

void FractalRenderer::renderImage() {

    auto now = chrono::system_clock::now();
    auto local_time = chrono::current_zone()->to_local(now);
    string timeStamp = format("{:%Y-%m-%d_%H-%M-%S}", local_time);
    string fileName = "output_" + timeStamp + ".png";

    fs::create_directories("output");
    fs::path destination = string("output/" + fileName);
    string destinationStr = destination.string();
    const char* destinationCstr = destinationStr.c_str();

    stbi_flip_vertically_on_write(true);
    stbi_write_png(destinationCstr, width, height, 3, buffer, width * 3);

}