#pragma once
#include "Window.h"

#include "imgui.h"
#include "rlImGui.h"

using namespace std;

Window::Window(const char* title, int i_width, int i_height) : width(i_width), height(i_height) {
	
	InitWindow(width, height, title);
	SetTargetFPS(60);

	Image dummyImage = { nullptr, width, height, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
	screenBuffer = LoadTextureFromImage(dummyImage);

}

Window::~Window() {

	CloseWindow();

}

void Window::Draw(FractalRenderer& fractalRenderer) {

	ClearBackground(BLACK);
	UpdateTexture(screenBuffer, fractalRenderer.buffer.data());
	DrawTexture(screenBuffer, 0, 0, WHITE);

	ZoomHandler(fractalRenderer);

}

void Window::ZoomHandler(FractalRenderer& fractalRenderer) {

	if (ImGui::GetIO().WantCaptureMouse) return;

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