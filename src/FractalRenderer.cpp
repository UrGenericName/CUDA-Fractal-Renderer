#pragma once
#include "FractalRenderer.h"

#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace fs = std::filesystem;
using namespace std;

#ifndef USING_CUDA
FractalRenderer::FractalRenderer(int i_width, int i_height) : width(i_width), height(i_height) {

    size_t N = 3 * width * height;
    buffer = static_cast<char*>(malloc(N * sizeof(char)));

}
#endif

FractalRenderer& FractalRenderer::operator=(const FractalRenderer& other) {

    maxIterations = other.maxIterations;
    fractalType = other.fractalType;
    renderMethod = other.renderMethod;

    juliaCx = other.juliaCx;
    juliaCy = other.juliaCy;

    posX = other.posX;
    posY = other.posY;

    scale = other.scale;

    return *this;

}

void FractalRenderer::generate(char* buffer) {

    switch (renderMethod) {

    case RenderMethod::CPU:
        generateCPU(buffer);
        break;

    case RenderMethod::CPU_MULTI_THREADED:
        generateMultiThreadedCPU(buffer);
        break;

    #ifdef USING_CUDA
    case RenderMethod::GPU:
        generateGPU(buffer);
        break;
    #endif

    }
}

void FractalRenderer::generate() {

    if (dynamicIterations) maxIterations = max(-log(scale) * dynamicIterations_c + dynamicIterations_min, dynamicIterations_min);

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

#ifndef USING_CUDA
void FractalRenderer::generateGPU(char* buffer) {}
#endif

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

void FractalRenderer::screenCoordToGlobal(Real x_screen, Real y_screen, Real* x_global, Real* y_global) {

    Real x_normalized = (static_cast<Real>(x_screen) / static_cast<Real>(width)) * 2.0f - 1.0f;
    Real y_normalized = -((static_cast<Real>(y_screen) / static_cast<Real>(height)) * 2.0f - 1.0f);

    // Slightly increases the size of either x or y to account for non-square aspect ratio
    if (width > height) {
        Real ratio = (static_cast<Real>(width) / height);
        x_normalized *= ratio;
    }
    else {
        Real ratio = (static_cast<Real>(height) / width);
        y_normalized *= ratio;
    }

    *x_global = x_normalized * scale + posX;
    *y_global = y_normalized * scale + posY;

}

void FractalRenderer::globalCoordToScreen(Real x_global, Real y_global, Real* x_screen, Real* y_screen) {

    Real x_normalized = (x_global - posX) / scale;
    Real y_normalized = -(y_global - posY) / scale;

    if (width > height) {
        Real ratio = (static_cast<Real>(width) / height);
        x_normalized /= ratio;
    }
    else {
        Real ratio = (static_cast<Real>(height) / width);
        y_normalized /= ratio;
    }

    *x_screen = ((x_normalized + 1.0f) / 2.0f) * static_cast<Real>(width);
    *y_screen = ((y_normalized + 1.0f) / 2.0f) * static_cast<Real>(height);
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

        float hue = ((float)iteration)/ (float)maxIterations;
        unsigned char brightness = static_cast<unsigned char>(255.0f * hue);

        color = { 0, 0, brightness, 255 };

        Vector3 getHue = colorToHSV(color);
        color = HSVtoColor(Vector3(getHue.x + ((1.0f - hue) * 360.0f), getHue.y, -((getHue.z - 1.0f) * (getHue.z - 1.0f)) + 1));

    }

    return color;

}

inline Vector3 FractalRenderer::colorToHSV(Color color) {

    int r = color.r;
    int g = color.g;
    int b = color.b;

    Vector3 hsv;

    // Normalize RGB
    double r_prime = r / 255.0f;
    double g_prime = g / 255.0f;
    double b_prime = b / 255.0f;

    // Find min and max channels
    double c_max = max({ r_prime, g_prime, b_prime });
    double c_min = min({ r_prime, g_prime, b_prime });
    double delta = c_max - c_min;

    // Calculate Hue
    if (delta == 0.0f) {
        hsv.x = 0.0f; // Achromatic case (Gray, White, Black)
    }
    else if (c_max == r_prime) {
        hsv.x = 60.0f * (g_prime - b_prime) / delta;
    }
    else if (c_max == g_prime) {
        hsv.x = 60.0f * (((b_prime - r_prime) / delta) + 2.0f);
    }
    else if (c_max == b_prime) {
        hsv.x = 60.0f * (((r_prime - g_prime) / delta) + 4.0f);
    }

    // Wrap negative hue angles back into the positive 0-360 range
    if (hsv.x < 0.0f) {
        hsv.x += 360.0f;
    }

    // Calculate Saturation
    if (c_max == 0.0f) {
        hsv.y = 0.0f;
    }
    else {
        hsv.y = delta / c_max;
    }

    // Calculate Value
    hsv.z = c_max;

    return hsv;

}

inline Color FractalRenderer::HSVtoColor(Vector3 HSV) {

    Color rgb{ 0, 0, 0 };

    float h = HSV.x;
    float s = HSV.y;
    float v = HSV.z;

    s = clamp(s, 0.0f, 1.0f);
    v = clamp(v, 0.0f, 1.0f);

    // Handle wrap-around for the hue angle
    if (h >= 360.0f) h = fmod(h, 360.0f);
    if (h < 0.0f)   h = fmod(h, 360.0f) + 360.0f;

    // Determine the sector on the 6-part color wheel (0 to 5)
    double sector_float = h / 60.0f;
    int sector = static_cast<int>(std::floor(sector_float));
    double fractional_part = sector_float - sector;

    double p = v * (1.0f - s);
    double q = v * (1.0f - (s * fractional_part));
    double t = v * (1.0f - (s * (1.0f - fractional_part)));

    double r_prime = 0.0f, g_prime = 0.0f, b_prime = 0.0f;

    // Assign primary and secondary colors depending on the hue sector
    switch (sector) {
    case 0: r_prime = v; g_prime = t; b_prime = p; break; // Red/Yellow sector
    case 1: r_prime = q; g_prime = v; b_prime = p; break; // Yellow/Green sector
    case 2: r_prime = p; g_prime = v; b_prime = t; break; // Green/Cyan sector
    case 3: r_prime = p; g_prime = q; b_prime = v; break; // Cyan/Blue sector
    case 4: r_prime = t; g_prime = p; b_prime = v; break; // Blue/Magenta sector
    case 5: r_prime = v; g_prime = p; b_prime = q; break; // Magenta/Red sector
    default: break;
    }

    // Scale components back to standard 8-bit integers with rounding (+0.5)
    rgb.r = static_cast<int>(r_prime * 255.0f + 0.5f);
    rgb.g = static_cast<int>(g_prime * 255.0f + 0.5f);
    rgb.b = static_cast<int>(b_prime * 255.0f + 0.5f);

    return rgb;

}

void FractalRenderer::renderImage(string fileName) {

    fs::create_directories("output");
    fs::path destination = string("output/" + fileName);
    string destinationStr = destination.string();
    const char* destinationCstr = destinationStr.c_str();

    stbi_flip_vertically_on_write(true);
    stbi_write_png(destinationCstr, width, height, 3, buffer, width * 3);

}

void FractalRenderer::renderImage() {

    auto now = chrono::system_clock::now();
    auto local_time = chrono::current_zone()->to_local(now);
    string timeStamp = format("{:%Y-%m-%d_%H-%M-%S}", local_time);
    string fileName = "output_" + timeStamp + ".png";

    renderImage(fileName);

}

void FractalRenderer::benchmark() {

    chrono::time_point<chrono::high_resolution_clock> start, end;

    // CPU BENCHMARK
    start = chrono::high_resolution_clock::now();
    generateCPU(buffer);
    end = chrono::high_resolution_clock::now();
    auto durationCPU = chrono::duration_cast<chrono::microseconds>(end - start);

    // MULTI-THREADED CPU BENCHMARK
    start = chrono::high_resolution_clock::now();
    generateMultiThreadedCPU(buffer);
    end = chrono::high_resolution_clock::now();
    auto duration_MultiThreaded_CPU = chrono::duration_cast<chrono::microseconds>(end - start);

    // GPU BENCHMARK
    #ifdef USING_CUDA
    start = chrono::high_resolution_clock::now();
    generateGPU(buffer);
    end = chrono::high_resolution_clock::now();
    auto duration_GPU = chrono::duration_cast<chrono::microseconds>(end - start);
    #endif

    cout << endl << endl << endl;
    cout << "    RENDER METHOD     |  TIME (microseconds)" << endl;
    cout << "----------------------+--------------------" << endl;
    cout << "CPU:                  | " + to_string(durationCPU.count()) + " us" << endl;
    cout << "----------------------+--------------------" << endl;
    cout << "CPU (multi-threaded): | " + to_string(duration_MultiThreaded_CPU.count()) + " us" << endl;

    #ifdef USING_CUDA
    cout << "----------------------+--------------------" << endl;
    cout << "GPU (CUDA):           | " + to_string(duration_GPU.count()) + " us" << endl;
    #endif
}