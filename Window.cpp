#pragma once
#include "Window.h"

#include <assert.h>

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

	CLASS_NAME = title;

	WNDCLASS wndClass = {};
	wndClass.lpszClassName = CLASS_NAME;
	wndClass.hInstance = m_hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.lpfnWndProc = WindowProc;

	RegisterClass(&wndClass);

	DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	RECT rect;
	rect.left = 250;
	rect.top = 250;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	AdjustWindowRect(&rect, style, false);

	m_hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		title,
		style,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		m_hInstance,
		NULL
	);

	ShowWindow(m_hWnd, SW_SHOW);
}

Window::~Window() {
	UnregisterClass(CLASS_NAME, m_hInstance);
}

void Window::Draw(std::vector<Color>& buffer) {

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