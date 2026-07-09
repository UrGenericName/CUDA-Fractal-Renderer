#pragma once
#include <vector>
#include <complex>
#include "raylib.h"

class Window;

class FractalRenderer {
public:
	
	friend Window;

	enum class FractalType {
		MANDELBROT,
		JULIA
	};

	enum class RenderMethod {
		CPU,
		CPU_MULTI_THREADED,
		GPU
	};

	int maxIterations = 10;
	FractalType fractalType = FractalType::MANDELBROT;
	RenderMethod renderMethod = RenderMethod::CPU_MULTI_THREADED;

	Vector2 juliaC{ 0.0f, 0.0f };

	Vector2 pos{ 0.0f, 0.0f };
	float scale = 1.0f;

	FractalRenderer(int i_width, int i_height); // implementation in .cu
	void generate(char* buffer);
	void generate();

private:
	const int width;
	const int height;

	void generateCPU(char* buffer);
	void generateMultiThreadedCPU(char* buffer);
	void generateGPU(char* buffer);	// implementation in .cu

	void renderPixels(char* buffer, unsigned int offset, unsigned int pixelCount);
	void renderMandelbrotSet(char* buffer, unsigned int offset, unsigned int pixelCount);
	void renderJuliaSet(char* buffer, unsigned int offset, unsigned int pixelCount);

	Vector2 screenCoordToGlobal(float x, float y);
	Vector2 globalCoordToScreen(float x, float y);

	inline int mandelbrotSetMath(float x, float y);
	inline int juliaSetMath(float x, float y);
	inline Color calculateColor(int iteration);

	char* buffer;	// R8G8B8 for each pixel
};