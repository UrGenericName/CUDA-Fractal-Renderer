#pragma once

#include "imgui.h"
#include "rlImGui.h"
#include "FractalRenderer.h"
#include "Animation.h"

#define MAX_ITERATIONS_MAX_VALUE 10000
#define MAX_DYNAMIC_ITERATIONS_C_MAX_VALUE 500
#define MAX_FRAMES_MAX_VALUE 1200

class DebugWindow {
public:
	DebugWindow();
	~DebugWindow();

	void Draw(FractalRenderer& fractalRenderer, Animation& animationComponent);

private:
	void DrawViewportInfo(FractalRenderer& fractalRenderer);
	void DrawRenderSettings(FractalRenderer& fractalRenderer);
	void DrawFractalSettings(FractalRenderer& fractalRenderer);
	void DrawAnimationSettings(FractalRenderer& fractalRenderer, Animation& animationComponent);

	bool drawWindow = true;

};