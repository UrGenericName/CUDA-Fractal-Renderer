#pragma once
#include "FractalRenderer.h"

#include <cuda_runtime.h>
#include <device_launch_parameters.h>

__device__ void colorToHSV(char* r, char* g, char* b, float* h, float* s, float* v) {
    // Normalize
    float r_prime = (float)(unsigned char)*r / 255.0f;
    float g_prime = (float)(unsigned char)*g / 255.0f;
    float b_prime = (float)(unsigned char)*b / 255.0f;

    // Find min and max channels using float intrinsics
    float c_max = fmaxf(r_prime, fmaxf(g_prime, b_prime));
    float c_min = fminf(r_prime, fminf(g_prime, b_prime));
    float delta = c_max - c_min;

    // Calculate Hue 
    if (delta < 1e-5f) { // Safe epsilon check for float precision to prevent missing red boundaries
        *h = 0.0f;
    }
    else if (c_max == r_prime) {
        *h = 60.0f * (g_prime - b_prime) / delta;
    }
    else if (c_max == g_prime) {
        *h = 60.0f * (((b_prime - r_prime) / delta) + 2.0f);
    }
    else if (c_max == b_prime) {
        *h = 60.0f * (((r_prime - g_prime) / delta) + 4.0f);
    }

    if (*h < 0.0f) {
        *h += 360.0f;
    }

    // Calculate Saturation 
    if (c_max == 0.0f) {
        *s = 0.0f;
    }
    else {
        *s = delta / c_max;
    }

    // Calculate Value 
    *v = c_max;
}

__device__ void HSVtoColor(float h, float s, float v, char* r, char* g, char* b) {
    // Read input values from pointers
    float local_h = h;
    float local_s = s;
    float local_v = v;

    // Clamp saturation and value using CUDA intrinsics
    local_s = fminf(fmaxf(local_s, 0.0f), 1.0f);
    local_v = fminf(fmaxf(local_v, 0.0f), 1.0f);

    // Handle wrap-around for the hue angle using CUDA fmodf
    if (local_h >= 360.0f) local_h = fmodf(local_h, 360.0f);
    if (local_h < 0.0f) local_h = fmodf(local_h, 360.0f) + 360.0f;

    // Determine the sector on the 6-part color wheel (0 to 5)
    double sector_float = local_h / 60.0f;
    int sector = static_cast<int>(floorf(sector_float));
    double fractional_part = sector_float - sector;

    double p = local_v * (1.0f - local_s);
    double q = local_v * (1.0f - (local_s * fractional_part));
    double t = local_v * (1.0f - (local_s * (1.0f - fractional_part)));

    double r_prime = 0.0f, g_prime = 0.0f, b_prime = 0.0f;

    // Assign primary and secondary colors depending on the hue sector
    switch (sector) {
    case 0: r_prime = local_v; g_prime = t;       b_prime = p;       break;
    case 1: r_prime = q;       g_prime = local_v; b_prime = p;       break;
    case 2: r_prime = p;       g_prime = local_v; b_prime = t;       break;
    case 3: r_prime = p;       g_prime = q;       b_prime = local_v; break;
    case 4: r_prime = t;       g_prime = p;       b_prime = local_v; break;
    case 5: r_prime = local_v; g_prime = p;       b_prime = q;       break;
    default: break;
    }

    // Scale components back to standard 8-bit integers and write to output pointers
    *r = static_cast<int>(r_prime * 255.0f + 0.5f);
    *g = static_cast<int>(g_prime * 255.0f + 0.5f);
    *b = static_cast<int>(b_prime * 255.0f + 0.5f);
}

__device__ void calculateColorCUDA(int maxIterations, int iteration, char* r, char* g, char* b) {

    *r = 0; 
    *g = 0; 
    *b = 0; 

    if (iteration <= maxIterations) { 
        float hue = ((float)iteration) / (float)maxIterations; 
        
        char temp_r = 0;
        char temp_g = 0;
        char temp_b = static_cast<char>(static_cast<unsigned char>(255.0f * hue)); 

        float h, s, v; 
        colorToHSV(&temp_r, &temp_g, &temp_b, &h, &s, &v);

        h = h + ((1.0f - hue) * 360.0f); 
        v = -((v - 1.0f) * (v - 1.0f)) + 1.0f; 

        HSVtoColor(h, s, v, r, g, b); 
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

__device__ void screenCoordToGlobalCUDA(int width, int height, float scale, Real posX, Real posY, Real x_screen, Real y_screen, Real* x_global, Real* y_global) {

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

__global__ void renderMandelbrotSetCUDA(char* buffer, int width, int height, Real scale, Real posX, Real posY, int maxIterations) {

    int index = (blockIdx.x * 256) + threadIdx.x;

    if (index >= width * height) return;

    int x = index % width;
    int y = index / width;

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

    int index = (blockIdx.x * 256) + threadIdx.x;

    if (index >= width * height) return;

    int x = index % width;
    int y = index / width;

    size_t pixelStartIndex = 3 * (y * width + x);

    Real x_global, y_global;
    screenCoordToGlobalCUDA(width, height, scale, posX, posY, x, y, &x_global, &y_global);
    int iteration = juliaSetMathCUDA(maxIterations, x_global, y_global, juliaCx, juliaCy);

    char* r = &buffer[pixelStartIndex + 0];
    char* g = &buffer[pixelStartIndex + 1];
    char* b = &buffer[pixelStartIndex + 2];

    calculateColorCUDA(maxIterations, iteration, r, g, b);

}

FractalRenderer::FractalRenderer(int i_width, int i_height) : width(i_width), height(i_height) {

    size_t N = 3 * width * height;
    buffer = static_cast<char*>(malloc(N * sizeof(char)));
    cudaMalloc(&d_buffer, N * sizeof(char));

}

void FractalRenderer::generateGPU(char* buffer) {

    size_t N = 3 * width * height;

    int threadsPerBlock = 256;
    int numBlocks = (width * height) / threadsPerBlock + 1;

    switch (fractalType) {

    case FractalType::MANDELBROT:
        cudaDeviceSynchronize();
        renderMandelbrotSetCUDA<<<numBlocks, threadsPerBlock >>>(d_buffer, width, height, scale, posX, posY, maxIterations);
        cudaMemcpy(buffer, d_buffer, N * sizeof(char), cudaMemcpyDeviceToHost);
        cudaDeviceSynchronize();
        break;

    case FractalType::JULIA:
        cudaDeviceSynchronize();
        renderJuliaSetCUDA<<<width, height >>>(d_buffer, width, height, scale, posX, posY, juliaCx, juliaCy, maxIterations);
        cudaMemcpy(buffer, d_buffer, N * sizeof(char), cudaMemcpyDeviceToHost);
        cudaDeviceSynchronize();
        break;

    }

}