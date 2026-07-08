#pragma once
#include <vector>
#include "raylib.h"

class Window;

class FractalRenderer {
public:
	
	friend Window;

	int maxIterations = 10;

	Vector2 pos{ 0.0f, 0.0f };
	float scale = 1.0f;

	FractalRenderer(int i_width, int i_height);
	void generate(std::vector<Color>& buffer);
	void generate();

private:
	int width;
	int height;

	void generateCPU(std::vector<Color>& buffer);
	void generateMultiThreadedCPU(std::vector<Color>& buffer);

	void FractalRenderer::calculatePixelGroup(std::vector<Color>& buffer, unsigned int offset, unsigned int pixelCount);

	std::vector<Color> buffer;
};