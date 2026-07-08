#pragma once
#include "DebugWindow.h"

using namespace ImGui;

DebugWindow::DebugWindow() { 
	assert(IsWindowReady()); // ensures raylib window is initalized first
	rlImGuiSetup(true); 
}

DebugWindow::~DebugWindow() { rlImGuiShutdown(); }

void DebugWindow::Draw(FractalRenderer& fractalRenderer) {

	if (IsKeyPressed(KEY_F)) drawWindow = !drawWindow;

	if (!drawWindow) return;

	rlImGuiBegin();

	if (BeginTable("Settings", 1)) {

		TableSetupColumn("Settings");
		TableHeadersRow();

		SliderInt("Iterations", &(fractalRenderer.maxIterations), 0, MAX_ITERATIONS_MAX_VALUE);

		EndTable();
	}

	rlImGuiEnd();

}