#pragma once
#include <vector>
#include "raylib.h"

class FractalRenderer {
public:
	
	Vector2 pos{ 0.0f, 0.0f };
	float scale = 1.0f;

	FractalRenderer(int i_width, int i_height) : width(i_width), height(i_height) {};
	void generate(std::vector<Color>& buffer);

private:
	int width;
	int height;

	int MAX_ITERATIONS = 100;
};