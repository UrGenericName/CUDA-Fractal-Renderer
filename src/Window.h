#pragma once

#include <Windows.h>
#include <vector>
#include "Color.h"
#include "FractalRenderer.h"

#define DRAG_COLOR RGB(0, 255, 255)
#define DRAG_OPACITY 75

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Window {
public:

	int const width;
	int const height;

	Window(const wchar_t* title = L"Window", int i_width = 640, int i_height = 480);
	void Draw(std::vector<Color>& buffer);
	~Window();

	bool ProcessMessages();
	void ZoomDragHandler(FractalRenderer& fractalRenderer);

private:

	const wchar_t* PARENT_CLASS_NAME = L"PARENT";
	const wchar_t* SIDE_PANEL_CLASS_NAME = L"SIDE_PANEL";

	HINSTANCE m_hInstance;
	HWND m_hWnd;
	HWND m_hSidePanel;

	void CreateParent(const wchar_t* title);
	void CreateSidePanel();
	void DrawDragBox(POINT initalCursorPos, POINT endCursorPos);

};