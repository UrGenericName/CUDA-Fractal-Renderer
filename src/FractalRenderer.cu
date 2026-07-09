#pragma once
#include "FractalRenderer.h"

#include <cuda_runtime.h>

__global__ void renderMandelbrotSetCUDA(char* buffer, int width, int height, float scale, float posX, float posY) {

    buffer[0 + 0] = 0;
    buffer[0 + 1] = 255;
    buffer[0 + 2] = 0;

}

void renderJuliaSetCUDA(char* buffer, int width, int height, float scale, float posX, float posY) {}

void FractalRenderer::generateGPU(char* buffer) {

    switch (fractalType) {

    case FractalType::MANDELBROT:
        cudaDeviceSynchronize();
        renderMandelbrotSetCUDA<<<1, 1>>>(buffer, width, height, scale, pos.x, pos.y);
        cudaDeviceSynchronize();
        break;

    case FractalType::JULIA:
        cudaDeviceSynchronize();
        renderJuliaSetCUDA(buffer, width, height, scale, pos.x, pos.y);
        cudaDeviceSynchronize();
        break;

    }

}

FractalRenderer::FractalRenderer(int i_width, int i_height) : width(i_width), height(i_height) {

    size_t N = 3 * width * height;
    cudaMallocManaged(&buffer, N * sizeof(char));
    //buffer = static_cast<char*>(malloc(N * sizeof(char)));

}