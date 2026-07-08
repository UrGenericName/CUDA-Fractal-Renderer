#pragma once
#include <iostream>

#include "Window.h"
#include "raylib.h"
#include "DebugWindow.h"
#include "FractalRenderer.h"

void main() {

	Window window { "Window", 960, 540 };
	DebugWindow debugWindow;

	FractalRenderer fractalRenderer{ window.width, window.height };

	while (!WindowShouldClose()) {

		BeginDrawing();
		fractalRenderer.generate();
		window.Draw(fractalRenderer);
		debugWindow.Draw(fractalRenderer);
		EndDrawing();
	}

	rlImGuiShutdown();

	return;

}