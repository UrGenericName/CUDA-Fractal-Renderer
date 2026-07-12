#pragma once
#include "Window.h"

#include "imgui.h"
#include "rlImGui.h"
#include <chrono>
#include <format>

using namespace std;
using namespace std::chrono;

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

	static auto start = high_resolution_clock::now();

	ClearBackground(BLACK);
	UpdateTexture(screenBuffer, fractalRenderer.buffer);
	DrawTexture(screenBuffer, 0, 0, WHITE);

	JuliaHandler(fractalRenderer);
	ZoomHandler(fractalRenderer);

	auto end = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(end - start);

	SetWindowTitle(generateFullTitle(duration.count()).c_str());

	start = end;

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

		Vector2 cursor_diff_normalized = { (initalCursorPos.x - currentCursorPos.x) / width * 2.0f, -(initalCursorPos.y - currentCursorPos.y) / height * 2.0f };


		if (width > height) {
			Real ratio = (static_cast<Real>(width) / height);
			cursor_diff_normalized.x *= ratio;
		}
		else {
			Real ratio = (static_cast<Real>(height) / width);
			cursor_diff_normalized.y *= ratio;
		}

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

string Window::generateFullTitle(double frameTime) {

	static vector<unsigned int> FPS_avg_vector(0);
	static unsigned int FPS_vector_sum = 0;

	unsigned int FPS = 1 / (frameTime * pow(10, -6));
	FPS_avg_vector.push_back(FPS);
	FPS_vector_sum += FPS;

	string FPS_avg_string = "N/A";

	if (FPS_avg_vector.size() > FPS_AVERAGE_SAMPLES) {
		FPS_vector_sum -= FPS_avg_vector[0];
		FPS_avg_vector.erase(FPS_avg_vector.begin());
		FPS_avg_string = to_string(FPS_vector_sum / FPS_avg_vector.size());
	}

	return format("{:<20} FPS: {:<8} FPS (avg): {}", title, to_string(FPS), FPS_avg_string);
}