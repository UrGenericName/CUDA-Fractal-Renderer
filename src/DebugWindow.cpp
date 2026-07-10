#pragma once
#include "DebugWindow.h"

#include "string"
#include <typeinfo>

using namespace ImGui;
using namespace std;

#define USING_CUDA

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
		DrawViewportInfo(fractalRenderer);
		DrawRenderSettings(fractalRenderer);
		DrawFractalSettings(fractalRenderer);
	}

	rlImGuiEnd();

}

void DebugWindow::DrawViewportInfo(FractalRenderer& fractalRenderer) {

	if (BeginTable("##ViewportInfoTable", 1)) {

		TableSetupColumn("Viewport");
		TableHeadersRow();

		#ifndef USING_CUDA

		TableNextColumn();
		TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "* CUDA not installed");
		TableNextRow();

		#endif

		TableNextColumn();
		string precisionText = string{ "Precision: " } + typeid(Real).name();
		Text(precisionText.c_str());
		TableNextRow();

		float total_width = GetContentRegionAvail().x;
		float item_width = (total_width - GetStyle().ItemSpacing.x) / 5.0f;

		TableNextColumn();
		SetNextItemWidth(item_width);
		Text("(x, y): ");
		SameLine();
		SetNextItemWidth(item_width);
		Text(to_string(fractalRenderer.posX).c_str());
		SameLine();
		SetNextItemWidth(item_width);
		Text(to_string(fractalRenderer.posY).c_str());

		TableNextRow();

		TableNextColumn();
		SetNextItemWidth(item_width);
		Text("Scale: ");
		SameLine();
		SetNextItemWidth(item_width);
		Text(to_string(fractalRenderer.scale).c_str());

		EndTable();

	}

	Spacing();

}

void DebugWindow::DrawRenderSettings(FractalRenderer& fractalRenderer) {

	if (BeginTable("##RenderSettingsTable", 1)) {

		TableSetupColumn("Render Settings");
		TableHeadersRow();

		TableNextColumn();

		SliderInt("Iterations", &(fractalRenderer.maxIterations), 0, MAX_ITERATIONS_MAX_VALUE);

		#ifdef USING_CUDA
		const char* methods[] = { "CPU", "CPU Multi-threaded", "GPU (CUDA)" };
        #else
		const char* methods[] = { "CPU", "CPU Multi-threaded" };
		#endif
		static int current_item_id = 0;

		if (BeginCombo("Method", methods[current_item_id])) {

			for (int n = 0; n < IM_ARRAYSIZE(methods); n++) {
				const bool is_selected = (current_item_id == n);

				if (ImGui::Selectable(methods[n], is_selected)) {
					current_item_id = n;
				}

				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			fractalRenderer.renderMethod = static_cast<FractalRenderer::RenderMethod>(current_item_id);

			ImGui::EndCombo();
		}
		
		if (Button("Benchmark")) fractalRenderer.benchmark();
		SameLine();
		if (Button("Render Image")) fractalRenderer.renderImage();

		EndTable();

	}

	Spacing();

}

void DebugWindow::DrawFractalSettings(FractalRenderer& fractalRenderer) {

	if (BeginTable("##FractalSettingsTable", 1)) {

		TableSetupColumn("Fractal Settings");
		TableHeadersRow();

		TableNextColumn();

		const char* types[] = { "Mandelbrot Set", "Julia Set" };
		static int current_type_id = 0;

		if (BeginCombo("Type", types[current_type_id])) {

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

		if (fractalRenderer.fractalType == FractalRenderer::FractalType::JULIA) {

			float total_width = GetContentRegionAvail().x;
			float item_width = (total_width - GetStyle().ItemSpacing.x) / 5.0f;

			double inputJuliaX = fractalRenderer.juliaCx;
			double inputJuliaY = fractalRenderer.juliaCy;

			SetNextItemWidth(item_width);
			Text("(x, y): ");
			SameLine();
			SetNextItemWidth(item_width);
			InputDouble("##juliaX", &(inputJuliaX));
			SameLine();
			SetNextItemWidth(item_width);
			InputDouble("##juliaY", &(inputJuliaY));

			fractalRenderer.juliaCx = inputJuliaX;
			fractalRenderer.juliaCy = inputJuliaY;

		}

		EndTable();
	}

}