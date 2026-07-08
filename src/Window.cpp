#pragma once
#include "Window.h"

using namespace std;

Window::Window(const char* title, int i_width, int i_height) : width(i_width), height(i_height) {
	
	InitWindow(width, height, title);
	SetTargetFPS(60);

}

Window::~Window() {

	CloseWindow();

}

void Window::Draw(std::vector<Color>& buffer) {

	ClearBackground(BLACK);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {

			DrawPixel(x, y, buffer[y * width + x]);

		}
	}

}

void Window::ZoomHandler(FractalRenderer& fractalRenderer) {

	// DRAGGING
	static bool firstClick = true;
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) 
	{

		static Vector2 initalCursorPos;
		static Vector2 origFratcalPos;
		if (firstClick) {
			initalCursorPos = GetMousePosition();
			origFratcalPos = fractalRenderer.pos;
			firstClick = false;
		}

		Vector2 currentCursorPos = GetMousePosition();

		Vector2 cursor_diff_normalized = { (initalCursorPos.x - currentCursorPos.x) / width, (initalCursorPos.y - currentCursorPos.y) / height };
		fractalRenderer.pos.x = origFratcalPos.x + (cursor_diff_normalized.x * fractalRenderer.scale);
		fractalRenderer.pos.y = origFratcalPos.y + (cursor_diff_normalized.y * fractalRenderer.scale);

	} 
	else 
	{
		firstClick = true;
	}

	// SCALING
	fractalRenderer.scale -= (GetMouseWheelMove() * zoomSpeed * fractalRenderer.scale);

}