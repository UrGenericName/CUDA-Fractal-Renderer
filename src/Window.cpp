#pragma once
#include "Window.h"

#include <assert.h>
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
	rect.left = 250;
	rect.top = 250;
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