#pragma once
#include <iostream>

#include "Window.h"
#include "Fractal_Renderer.cuh"

void main() {
	
	Window* window = new Window();

	std::vector<Color> buffer(480 * 640);

	generateFractalGPU(buffer, window->width, window->height);

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