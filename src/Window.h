#pragma once

#include <vector>
#include "raylib.h"
#include "FractalRenderer.h"

#define DRAG_ALPHA 100
#define DRAG_COLOR Color{ 0, 0, 255, DRAG_ALPHA }
#define DRAG_BORDER_COLOR Color{ 0, 0, 255, 255 }

class Window {
public:

	const int width;
	const int height;

	Window(const char* title, int i_width, int i_height);
	~Window();

	void Draw(std::vector<Color>& buffer);
	void ZoomHandler(FractalRenderer& fractalRenderer);

private:

	float zoomSpeed = 0.05;

};