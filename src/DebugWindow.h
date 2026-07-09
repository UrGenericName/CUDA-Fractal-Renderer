#pragma once

#include "imgui.h"
#include "rlImGui.h"
#include "FractalRenderer.h"

#define MAX_ITERATIONS_MAX_VALUE 1000

class DebugWindow {
public:
	DebugWindow();
	~DebugWindow();

	void Draw(FractalRenderer& fractalRenderer);

private:
	void DrawViewportInfo(FractalRenderer& fractalRenderer);
	void DrawRenderSettings(FractalRenderer& fractalRenderer);
	void DrawFractalSettings(FractalRenderer& fractalRenderer);

	bool drawWindow = true;

};