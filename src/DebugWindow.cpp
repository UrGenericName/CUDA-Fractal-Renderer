#pragma once
#include "DebugWindow.h"

using namespace ImGui;

DebugWindow::DebugWindow() { 
	assert(IsWindowReady()); // ensures raylib window is initalized first
	rlImGuiSetup(true); 
}

DebugWindow::~DebugWindow() { 
	rlImGuiShutdown(); 
}

void DebugWindow::Draw(FractalRenderer& fractalRenderer) {

	if (IsKeyPressed(KEY_F)) drawWindow = !drawWindow;

	rlImGuiBegin();

	if (drawWindow) {

		if (BeginTable("Settings", 1)) {

			TableSetupColumn("Settings");
			TableHeadersRow();

			SliderInt("Iterations", &(fractalRenderer.maxIterations), 0, MAX_ITERATIONS_MAX_VALUE);

			const char* items[] = { "CPU", "CPU Multi-threaded", "GPU (CUDA)" };
			static int current_item_id = 0;

			if (BeginCombo("Render Method", items[current_item_id])) {

				for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
					const bool is_selected = (current_item_id == n);

					if (ImGui::Selectable(items[n], is_selected)) {
						current_item_id = n;
					}

					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				fractalRenderer.renderMethod = static_cast<FractalRenderer::RenderMethod>(current_item_id);

				ImGui::EndCombo();
			}

			const char* types[] = { "Mandelbrot Set", "Julia Set" };
			static int current_type_id = 0;

			if (BeginCombo("Fractal Type", types[current_type_id])) {

				for (int n = 0; n < IM_ARRAYSIZE(types); n++) {
					const bool is_selected = (current_type_id == n);

					if (ImGui::Selectable(types[n], is_selected)) {
						current_type_id = n;
					}

					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				fractalRenderer.fractalType = static_cast<FractalRenderer::FractalType>(current_type_id);

				ImGui::EndCombo();
			}

			SliderFloat("x", &(fractalRenderer.juliaC.x), -5.0f, 5.0f);
			SliderFloat("y", &(fractalRenderer.juliaC.y), -5.0f, 5.0f);

			EndTable();
		}

	}

	rlImGuiEnd();

}