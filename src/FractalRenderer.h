#pragma once
#include <vector>
#include <string>
#include "raylib.h"

using Real = float;

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
	RenderMethod renderMethod = RenderMethod::CPU;

	Real juliaCx = 0.0f;
	Real juliaCy = 0.0f;

	Real posX = 0.0f;
	Real posY = 0.0f;

	float scale = 1.0f;

	FractalRenderer(int i_width, int i_height); // implementation in .cu

	FractalRenderer& operator=(const FractalRenderer& other);

	void generate(char* buffer);
	void generate();
	void renderImage(std::string fileName);
	void renderImage();
	void benchmark();

private:
	const int width;
	const int height;

	void generateCPU(char* buffer);
	void generateMultiThreadedCPU(char* buffer);
	void generateGPU(char* buffer);	// implementation in .cu

	void renderPixels(char* buffer, unsigned int offset, unsigned int pixelCount);
	void renderMandelbrotSet(char* buffer, unsigned int offset, unsigned int pixelCount);
	void renderJuliaSet(char* buffer, unsigned int offset, unsigned int pixelCount);

	void screenCoordToGlobal(Real x, Real y, Real* x_global, Real* y_global);
	void globalCoordToScreen(Real x, Real y, Real* x_screen, Real* y_screen);

	inline int mandelbrotSetMath(int maxIterations, Real x, Real y);
	inline int juliaSetMath(int maxIterations, Real x, Real y, Real juliaCx, Real juliaCy);
	inline Color calculateColor(int maxIterations, int iteration);

	char* buffer;	// R8G8B8 for each pixel
};