#pragma once
#include <iostream>
#include "raylib.h"

#include "Window.h"
#include "DebugWindow.h"
#include "FractalRenderer.h"
#include "Animation.h"

#define WIDTH 1500 
#define HEIGHT 960

void main() {

	Window window { "Fractal Renderer", WIDTH, HEIGHT };
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