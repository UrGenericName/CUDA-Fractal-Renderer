#pragma once
#include <iostream>

#include "Window.h"
#include "raylib.h"
#include "DebugWindow.h"
#include "FractalRenderer.h"
#include "Animation.h"

void main() {

	Window window { "Fractal Renderer", 860 * 2, 480 * 2 };
	DebugWindow debugWindow;

	FractalRenderer fractalRenderer{ window.width, window.height};
	Animation animationComponent;

	while (!WindowShouldClose()) {

		fractalRenderer.generate();
		animationComponent.Animate(fractalRenderer);

		BeginDrawing();
		window.Draw(fractalRenderer);
		debugWindow.Draw(fractalRenderer, animationComponent);
		EndDrawing();
	}

	return;

}