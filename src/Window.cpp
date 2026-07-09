#pragma once
#include "Window.h"

#include "imgui.h"
#include "rlImGui.h"

using namespace std;

Window::Window(string i_title, int i_width, int i_height) : title(i_title), width(i_width), height(i_height) {
	
	InitWindow(width, height, generateFullTitle().c_str());
	SetTargetFPS(9999);

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
	fractalRenderer.screenCoordToGlobal(mousePos.x, mousePos.y, &fractalRenderer.juliaCx, &fractalRenderer.juliaCy);

}

void Window::DrawJuliaCursor(FractalRenderer& fractalRenderer) {

	Real screenX, screenY;
	fractalRenderer.globalCoordToScreen(fractalRenderer.juliaCx, fractalRenderer.juliaCy, &screenX, &screenY);

	const int length = 11;
	const int thickness = 1;

	Rectangle verticalBar;
	verticalBar.width = thickness;
	verticalBar.height = length;
	verticalBar.x = screenX - (thickness / 2);
	verticalBar.y = screenY - (length / 2);

	Rectangle horizontalBar;
	horizontalBar.width = length;
	horizontalBar.height = thickness;
	horizontalBar.x = screenX - (length / 2);
	horizontalBar.y = screenY - (thickness / 2);

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
		static Real origFractalPosX, origFractalPosY;
		if (firstClick) {
			initalCursorPos = GetMousePosition();
			origFractalPosX = fractalRenderer.posX;
			origFractalPosY = fractalRenderer.posY;
			firstClick = false;
		}

		Vector2 currentCursorPos = GetMousePosition();

		Vector2 cursor_diff_normalized = { (initalCursorPos.x - currentCursorPos.x) / width, (initalCursorPos.y - currentCursorPos.y) / height };
		fractalRenderer.posX = origFractalPosX + (cursor_diff_normalized.x * fractalRenderer.scale);
		fractalRenderer.posY = origFractalPosY + (cursor_diff_normalized.y * fractalRenderer.scale);

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