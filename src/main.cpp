#pragma once
#include <iostream>

#include "Window.h"
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "FractalRenderer.h"

void main() {

	Window window { "Window", 960, 540 };
	FractalRenderer fractalRenderer{ window.width, window.height };

	std::vector<Color> buffer{ static_cast<unsigned int>(window.width * window.height) };

	while (!WindowShouldClose()) {

		BeginDrawing();
		fractalRenderer.generate(buffer);
		window.Draw(buffer);
		window.ZoomHandler(fractalRenderer);

		rlImGuiBegin();

		ImGui::Begin("Hello rlImGui");
		ImGui::Text("This works perfectly with Raylib!");
		ImGui::End();

		rlImGuiEnd();
		EndDrawing();
	}

	rlImGuiShutdown();

	return;

}