#pragma once
#include <iostream>

#include "Window.h"
#include "FractalRenderer.h"

void main() {
	
	Window* window = new Window();
	FractalRenderer fractalRenderer{ window->width, window->height };

	std::vector<Color> buffer(480 * 640);
	fractalRenderer.generate(buffer);

	window->Draw(buffer);

	bool running = true;
	while (running) {

		if (!window->ProcessMessages()) {
			running = false;
			std::cout << "Closing Window" << std::endl;
		}

		Sleep(10);

	}

	delete window;

	return;

}