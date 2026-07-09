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

	Window(std::string title, int i_width, int i_height);
	~Window();

	void Draw(FractalRenderer& fractalRenderer);

private:

	float zoomSpeed = 0.075;
	Texture2D screenBuffer;

	void ZoomHandler(FractalRenderer& fractalRenderer);
	std::string generateFullTitle();

	std::string title;
};