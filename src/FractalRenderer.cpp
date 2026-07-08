#pragma once
#include "FractalRenderer.h"

#include <complex>
#include <thread>

using namespace std;

FractalRenderer::FractalRenderer(int i_width, int i_height) : width(i_width), height(i_height) {
    buffer.resize(width * height);
}

void FractalRenderer::generate(vector<Color>& buffer) {
    generateMultiThreadedCPU(buffer);
}

void FractalRenderer::generate() {
    generate(this->buffer);
}

void FractalRenderer::generateCPU(vector<Color>& buffer) {

    calculatePixelGroup(buffer, 0, width * height);

}

void FractalRenderer::generateMultiThreadedCPU(vector<Color>& buffer) {

    vector<thread> threads;
    unsigned int threadCount = thread::hardware_concurrency();
    int pixelsPerThread = (width * height) / threadCount;

    // Calculate a group of pixel for each thread
    for (int i = 0; i < threadCount; i++) {
        
        int offset = i * pixelsPerThread;

        // for thread i, render a specific group of pixels
        threads.push_back(std::thread([&buffer, offset, pixelsPerThread, this]() {

            calculatePixelGroup(buffer, offset, pixelsPerThread);

        }));

    }

    // Wait for all them to join back
    for (auto& t : threads) {
        t.join();
    }

}

void FractalRenderer::calculatePixelGroup(vector<Color>& buffer, unsigned int offset, unsigned int pixelCount) {

    for (int i = offset; i < offset + pixelCount; ++i) {

        int x = i % width;
        int y = i / width;

        float x_normalized = (static_cast<float>(x) / static_cast<float>(width));
        float y_normalized = (static_cast<float>(y) / static_cast<float>(height));

        // Slightly increases the size of either x or y to account for non-square aspect ratio
        if (width > height) {
            x_normalized *= (static_cast<float>(width) / height);
        }
        else {
            y_normalized *= (static_cast<float>(height) / width);
        }

        float x_global = (x_normalized - 0.5f) * scale + 0.5f + pos.x;
        float y_global = (y_normalized - 0.5f) * scale + 0.5f + pos.y;

        complex<float> z{ 0.0f, 0.0f };
        complex<float> c{ x_global, y_global };

        int iteration = 0;
        while (iteration <= maxIterations) {

            z = (z * z) + c;

            if (real(z) * real(z) + imag(z) * imag(z) > 4) break;
            iteration++;
        }

        Color color = { 0, 0, 0, 255 };
        if (iteration <= maxIterations) color = { 0, static_cast<unsigned char>(255 * ((float)iteration / (float)maxIterations)), static_cast<unsigned char>(255 * ((float)iteration / (float)maxIterations)), 255 };

        buffer[y * width + x] = color;
    }

}