#include <iostream>

#include "Window.h"

void main() {
	
	Window* window = new Window();

	std::vector<Color> buffer(480 * 640);

	for (int y = 0; y < window->height; ++y) {
		for (int x = 0; x < window->width; ++x) {

			Color color = { static_cast<int>(255 * (float(x) / float(window->width))), static_cast<int>(255 * (float(y) / float(window->height))), 255};

			buffer[y * window->width + x] = color;

		}
	}

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