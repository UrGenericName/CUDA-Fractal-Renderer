#pragma once
#include <vector>
#include "raylib.h"

class Window;

class FractalRenderer {
public:
	
	friend Window;

	int maxIterations = 100;

	Vector2 pos{ 0.0f, 0.0f };
	float scale = 1.0f;

	FractalRenderer(int i_width, int i_height);
	void generate(std::vector<Color>& buffer);
	void generate();

private:
	int width;
	int height;

	std::vector<Color> buffer;
};