#pragma once
#pragma comment(lib, "Msimg32.lib") 
#include "Window.h"

#include <assert.h>
#include <iostream>
#include <string>

using namespace std;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg)
	{

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);

}

Window::Window(const wchar_t* title, int i_width, int i_height) : m_hInstance(GetModuleHandle(nullptr)), width(i_width), height(i_height) {

	CreateParent(L"Parent");
	CreateSidePanel();

	ShowWindow(m_hWnd, SW_SHOW);
}

Window::~Window() {
	UnregisterClass(PARENT_CLASS_NAME, m_hInstance);
}

void Window::CreateParent(const wchar_t* title) {

	WNDCLASS parentClass = {};
	parentClass.lpszClassName = PARENT_CLASS_NAME;
	parentClass.hInstance = m_hInstance;
	parentClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	parentClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	parentClass.lpfnWndProc = WindowProc;

	RegisterClass(&parentClass);

	DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	RECT rect;
	rect.left = 60;
	rect.top = 60;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	AdjustWindowRect(&rect, style, false);

	m_hWnd = CreateWindowEx(
		0,
		PARENT_CLASS_NAME, title,
		style,
		rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, m_hInstance, NULL
	);

}

void Window::CreateSidePanel() {

	WNDCLASS sidePanelClass = {};
	sidePanelClass.lpszClassName = SIDE_PANEL_CLASS_NAME;
	sidePanelClass.hInstance = m_hInstance;
	sidePanelClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	sidePanelClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	sidePanelClass.lpfnWndProc = WindowProc;

	RegisterClass(&sidePanelClass);

	DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	int sidebarWidth = 250;

	m_hSidePanel = CreateWindowEx(
		0,
		SIDE_PANEL_CLASS_NAME,
		L"",
		style,
		10, 10, sidebarWidth - 20, 200,
		m_hWnd, (HMENU)101, m_hInstance, NULL
	);

}

void Window::ZoomDragHandler(FractalRenderer& fractalRenderer) {

	if (GetForegroundWindow() != m_hWnd) return;

	static bool firstClick = true;
	static POINT initalCursorPos;
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) 
	{

		// INITAL POINT
		if (firstClick) {
			firstClick = false;

			GetCursorPos(&initalCursorPos);
			ScreenToClient(m_hWnd, &initalCursorPos);
		}

		// DRAG BOX
		POINT currentCursorPos;
		GetCursorPos(&currentCursorPos);
		ScreenToClient(m_hWnd, &currentCursorPos);

		DrawDragBox(initalCursorPos, currentCursorPos);
	}
	else 
	{	
		// END POINT
		if (!firstClick) {
			firstClick = true;

			POINT endCursorPos;
			GetCursorPos(&endCursorPos);
			ScreenToClient(m_hWnd, &endCursorPos);

			double x_local_min, x_local_max;
			x_local_max = max(static_cast<double>(initalCursorPos.x) / width, static_cast<double>(endCursorPos.x) / width) * 2.0f - 1.0f;
			x_local_min = min(static_cast<double>(initalCursorPos.x) / width, static_cast<double>(endCursorPos.x) / width) * 2.0f - 1.0f;

			double y_local_min, y_local_max;
			y_local_max = max(static_cast<double>(initalCursorPos.y) / height, static_cast<double>(endCursorPos.y) / height) * 2.0f - 1.0f;
			y_local_min = min(static_cast<double>(initalCursorPos.y) / height, static_cast<double>(endCursorPos.y) / height) * 2.0f - 1.0f;

			double x_current_global_pos = (fractalRenderer.x_min + fractalRenderer.x_max) / 2.0f;
			double y_current_global_pos = (fractalRenderer.y_min + fractalRenderer.y_max) / 2.0f;

			double x_current_global_max_centered = fractalRenderer.x_max - x_current_global_pos;
			double y_current_global_max_centered = fractalRenderer.y_max - y_current_global_pos;

			double x_new_global_max = x_local_max * x_current_global_max_centered + x_current_global_pos;
			double x_new_global_min = x_local_min * x_current_global_max_centered + x_current_global_pos;
			double y_new_global_max = y_local_max * y_current_global_max_centered + y_current_global_pos;
			double y_new_global_min = y_local_min * y_current_global_max_centered + y_current_global_pos;

			fractalRenderer.x_max = x_new_global_max;
			fractalRenderer.x_min = x_new_global_min;
			fractalRenderer.y_max = y_new_global_max;
			fractalRenderer.y_min = y_new_global_min;

		}
	}

}

void Window::DrawDragBox(POINT initalCursorPos, POINT endCursorPos) {

	HDC hdc = GetDC(m_hWnd);
	if (!hdc) return;

	// RECT VARIABLES
	int left = min(initalCursorPos.x, endCursorPos.x);
	int right = max(initalCursorPos.x, endCursorPos.x);
	int bottom = max(initalCursorPos.y, endCursorPos.y);
	int top = min(initalCursorPos.y, endCursorPos.y);

	int rectWidth = right - left;
	int rectHeight = bottom - top;

	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rectWidth, rectHeight);
	HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap);

	HBRUSH blueBrush = CreateSolidBrush(DRAG_COLOR);
	RECT tempRect = { 0, 0, rectWidth, rectHeight };
	FillRect(memDC, &tempRect, blueBrush);

	// TRANSPARENCY STUFF
	BLENDFUNCTION blend = {};
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.SourceConstantAlpha = DRAG_OPACITY;
	blend.AlphaFormat = 0;

	AlphaBlend(
		hdc, left, top, rectWidth, rectHeight,
		memDC, 0, 0, rectWidth, rectHeight,
		blend
	);

	// DEALLOCATE
	DeleteObject(blueBrush);
	SelectObject(memDC, oldBitmap);
	DeleteObject(memBitmap);
	DeleteDC(memDC);
	ReleaseDC(m_hWnd, hdc);

}

void Window::Draw(vector<Color>& buffer) {

	assert(buffer.size() == width * height);

	HDC screenDC = GetDC(m_hWnd);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;

	SetDIBitsToDevice(
		screenDC,
		0, 0,
		width, height,
		0, 0,
		0, height,
		buffer.data(),
		&bmi,
		DIB_RGB_COLORS
	);

	ReleaseDC(NULL, screenDC);

}

bool Window::ProcessMessages() {

	MSG msg = {};

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) 
	{	
		if (msg.message == WM_QUIT) return false;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;

}