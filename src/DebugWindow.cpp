#pragma once
#include "DebugWindow.h"

#include "string"
#include <typeinfo>
#include "magic_enum/magic_enum.hpp"

using namespace ImGui;
using namespace std;

DebugWindow::DebugWindow() { 
	assert(IsWindowReady()); // ensures raylib window is initalized first
	rlImGuiSetup(true); 
}

DebugWindow::~DebugWindow() { 
	rlImGuiShutdown(); 
}

void DebugWindow::Draw(FractalRenderer& fractalRenderer, Animation& animationComponent) {

	if (IsKeyPressed(KEY_F)) drawWindow = !drawWindow;

	rlImGuiBegin();

	if (drawWindow) {
		DrawViewportInfo(fractalRenderer);
		DrawRenderSettings(fractalRenderer);
		DrawFractalSettings(fractalRenderer);
		DrawAnimationSettings(fractalRenderer, animationComponent);
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

		// PRECISION INFO
		TableNextColumn();
		string precisionText = string{ "Precision: " } + typeid(Real).name();
		Text(precisionText.c_str());
		TableNextRow();

		float total_width = GetContentRegionAvail().x;
		float item_width = (total_width - GetStyle().ItemSpacing.x) / 5.0f;

		// POSITION
		TableNextColumn();
		double inputPosX = fractalRenderer.posX;
		double inputPosY = fractalRenderer.posY;

		SetNextItemWidth(item_width);
		Text("(x, y): ");
		SameLine();
		SetNextItemWidth(item_width);
		InputDouble("##posX", &(inputPosX));
		SameLine();
		SetNextItemWidth(item_width);
		InputDouble("##posY", &(inputPosY));

		fractalRenderer.posX = inputPosX;
		fractalRenderer.posY = inputPosY;
		TableNextRow();

		// SCALE
		TableNextColumn();
		float inputScale = fractalRenderer.scale;

		SetNextItemWidth(item_width);
		Text("Scale:  ");
		SameLine();
		SetNextItemWidth(item_width);
		InputFloat("##scale", &inputScale);

		fractalRenderer.scale = inputScale;

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

				if (Selectable(methods[n], is_selected)) {
					current_item_id = n;
				}

				if (is_selected) {
					SetItemDefaultFocus();
				}
			}

			fractalRenderer.renderMethod = static_cast<FractalRenderer::RenderMethod>(current_item_id);

			EndCombo();
		}
		
		if (Button("Benchmark")) fractalRenderer.benchmark();

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

				if (Selectable(types[n], is_selected)) {
					current_type_id = n;
				}

				if (is_selected) {
					SetItemDefaultFocus();
				}
			}

			fractalRenderer.fractalType = static_cast<FractalRenderer::FractalType>(current_type_id);

			EndCombo();
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

void DebugWindow::DrawAnimationSettings(FractalRenderer& fractalRenderer, Animation& animationComponent) {

	if (BeginTable("##AnimationSettingsTable", 1)) {

		TableSetupColumn("Animation Settings");
		TableHeadersRow();



		TableNextColumn();
		BeginDisabled(animationComponent.animationPreviewPhase != Animation::AnimationPhase::COMPLETE || animationComponent.animationVideoPhase != Animation::AnimationPhase::COMPLETE);

		if (Button("Render Image")) fractalRenderer.renderImage();
		SameLine();
		if (Button("Render Video")) animationComponent.animationVideoPhase = Animation::AnimationPhase::ANIMATING;

		SliderInt("Frames", &animationComponent.totalFrames, 1, 1000);


		constexpr auto enumNames = magic_enum::enum_names<Animation::Animations>();

		std::array<const char*, enumNames.size()> animations;
		for (size_t i = 0; i < enumNames.size(); ++i) {
			animations[i] = enumNames[i].data();
		}
		
		int current_item_id = static_cast<int>(animationComponent.animation);

		if (BeginCombo("Animation", animations[current_item_id])) {

			for (int n = 0; n < static_cast<int>(animations.size()); n++) {
				const bool is_selected = (current_item_id == n);

				if (Selectable(animations[n], is_selected)) {
					current_item_id = n;
				}

				if (is_selected) {
					SetItemDefaultFocus();
				}
			}

			animationComponent.animation = static_cast<Animation::Animations>(current_item_id);

			EndCombo();
		}


		if (Button("Preview")) animationComponent.animationPreviewPhase = Animation::AnimationPhase::ANIMATING;

		EndDisabled();

		SameLine();
		Text("Frame: %d/%d", animationComponent.currentFrame, animationComponent.totalFrames);

		EndTable();
	}

}