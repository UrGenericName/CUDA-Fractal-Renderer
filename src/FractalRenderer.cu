#pragma once
#include "FractalRenderer.h"

#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#define USING_CUDA

__device__ void calculateColorCUDA(int maxIterations, int iteration, char* r, char* g, char* b) {

    *r = 0;
    *g = 0;
    *b = 0;

    if (iteration <= maxIterations) {

        *b = 255;

    }

}

__device__ int mandelbrotSetMathCUDA(int maxIterations, Real x, Real y) {

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

__device__ int juliaSetMathCUDA(int maxIterations, Real x, Real y, Real juliaCx, Real juliaCy) {

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

__device__ void screenCoordToGlobalCUDA(int width, int height, float scale, Real posX, Real posY, Real x, Real y, Real* x_global, Real* y_global) {

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

__global__ void renderMandelbrotSetCUDA(char* buffer, int width, int height, Real scale, Real posX, Real posY, int maxIterations) {

    int x = blockIdx.x;
    int y = threadIdx.x;

    size_t pixelStartIndex = 3 * (y * width + x);

    Real x_global, y_global;
    screenCoordToGlobalCUDA(width, height, scale, posX, posY, x, y, &x_global, &y_global);
    int iteration = mandelbrotSetMathCUDA(maxIterations, x_global, y_global);

    char* r = &buffer[pixelStartIndex + 0];
    char* g = &buffer[pixelStartIndex + 1];
    char* b = &buffer[pixelStartIndex + 2];

    calculateColorCUDA(maxIterations, iteration, r, g, b);

}

__global__ void renderJuliaSetCUDA(char* buffer, int width, int height, Real scale, Real posX, Real posY, Real juliaCx, Real juliaCy, int maxIterations) {

    int x = blockIdx.x;
    int y = threadIdx.x;

    size_t pixelStartIndex = 3 * (y * width + x);

    Real x_global, y_global;
    screenCoordToGlobalCUDA(width, height, scale, posX, posY, x, y, &x_global, &y_global);
    int iteration = juliaSetMathCUDA(maxIterations, x_global, y_global, juliaCx, juliaCy);

    char* r = &buffer[pixelStartIndex + 0];
    char* g = &buffer[pixelStartIndex + 1];
    char* b = &buffer[pixelStartIndex + 2];

    calculateColorCUDA(maxIterations, iteration, r, g, b);

}

void FractalRenderer::generateGPU(char* buffer) {

    switch (fractalType) {

    case FractalType::MANDELBROT:
        cudaDeviceSynchronize();
        renderMandelbrotSetCUDA<<<width, height>>>(buffer, width, height, scale, posX, posY, maxIterations);
        cudaDeviceSynchronize();
        break;

    case FractalType::JULIA:
        cudaDeviceSynchronize();
        renderJuliaSetCUDA<<<width, height >>>(buffer, width, height, scale, posX, posY, juliaCx, juliaCy, maxIterations);
        cudaDeviceSynchronize();
        break;

    }

}

FractalRenderer::FractalRenderer(int i_width, int i_height) : width(i_width), height(i_height) {

    size_t N = 3 * width * height;
    cudaMallocManaged(&buffer, N * sizeof(char));
    //buffer = static_cast<char*>(malloc(N * sizeof(char)));

}