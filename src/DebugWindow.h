#pragma once

#include "imgui.h"
#include "rlImGui.h"
#include "FractalRenderer.h"
#include "Animation.h"

#define MAX_ITERATIONS_MAX_VALUE 2000

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