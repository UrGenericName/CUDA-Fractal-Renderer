#pragma once
#include <iostream>
#include "raylib.h"

#include "Window.h"
#include "DebugWindow.h"
#include "FractalRenderer.h"
#include "Animation.h"

void main() {

	Window window { "Fractal Renderer", 860, 480 };
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