#pragma once
#include <iostream>

#include "Window.h"
#include "raylib.h"
#include "DebugWindow.h"
#include "FractalRenderer.h"

void main() {

	Window window { "Fractal Renderer", 960, 540 };
	DebugWindow debugWindow;

	FractalRenderer fractalRenderer{ window.width, window.height};

	while (!WindowShouldClose()) {

		fractalRenderer.generate();

		BeginDrawing();
		window.Draw(fractalRenderer);
		debugWindow.Draw(fractalRenderer);
		EndDrawing();
	}

	return;

}