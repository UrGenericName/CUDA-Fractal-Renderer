#pragma once

#include <Windows.h>
#include <vector>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct Color {
	unsigned char b = 0;
	unsigned char g = 0;
	unsigned char r = 0;
};

class Window {
public:

	int const width;
	int const height;

	Window(const wchar_t* title = L"Window", int i_width = 640, int i_height = 480);
	void Draw(std::vector<Color>& buffer);
	~Window();

	bool ProcessMessages();

private:

	const wchar_t* CLASS_NAME;
	HINSTANCE m_hInstance;
	HWND m_hWnd;

};