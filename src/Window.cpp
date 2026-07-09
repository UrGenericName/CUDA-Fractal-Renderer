#pragma once
#include "Window.h"

#include "imgui.h"
#include "rlImGui.h"

using namespace std;

Window::Window(string i_title, int i_width, int i_height) : title(i_title), width(i_width), height(i_height) {
	
	InitWindow(width, height, generateFullTitle().c_str());

	Image dummyImage = { nullptr, width, height, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8 };
	screenBuffer = LoadTextureFromImage(dummyImage);

}

Window::~Window() {

	CloseWindow();

}

void Window::Draw(FractalRenderer& fractalRenderer) {

	ClearBackground(BLACK);
	UpdateTexture(screenBuffer, fractalRenderer.buffer);
	DrawTexture(screenBuffer, 0, 0, WHITE);

	JuliaHandler(fractalRenderer);
	ZoomHandler(fractalRenderer);

	SetWindowTitle(generateFullTitle().c_str());

}

void Window::JuliaHandler(FractalRenderer& fractalRenderer) {

	if (fractalRenderer.fractalType == FractalRenderer::FractalType::JULIA) {
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) SetJuliaCursor(fractalRenderer);
		DrawJuliaCursor(fractalRenderer);
	}

}

void Window::SetJuliaCursor(FractalRenderer& fractalRenderer) {

	Vector2 mousePos = GetMousePosition();
	fractalRenderer.juliaC = fractalRenderer.screenCoordToGlobal(mousePos.x, mousePos.y);

}

void Window::DrawJuliaCursor(FractalRenderer& fractalRenderer) {

	Vector2 screenCoord = fractalRenderer.globalCoordToScreen(fractalRenderer.juliaC.x, fractalRenderer.juliaC.y);

	const int length = 11;
	const int thickness = 1;

	Rectangle verticalBar;
	verticalBar.width = thickness;
	verticalBar.height = length;
	verticalBar.x = screenCoord.x - (thickness / 2);
	verticalBar.y = screenCoord.y - (length / 2);

	Rectangle horizontalBar;
	horizontalBar.width = length;
	horizontalBar.height = thickness;
	horizontalBar.x = screenCoord.x - (length / 2);
	horizontalBar.y = screenCoord.y - (thickness / 2);

	DrawRectangleRec(verticalBar, WHITE);
	DrawRectangleRec(horizontalBar, WHITE);

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

string Window::generateFullTitle() {
	return title + "     FPS: " + to_string(GetFPS());
}