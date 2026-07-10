#pragma once

#include "Animation.h"

#include <chrono>
#include <filesystem>
#include <numbers>

using namespace std;

void Animation::Animate(FractalRenderer& fractalRenderer) {

	AnimationPreviewHandler(fractalRenderer);
	AnimationVideoHandler(fractalRenderer);

}

void Animation::AnimationPreviewHandler(FractalRenderer& fractalRenderer) {

	static FractalRenderer* copy;

	if (currentFrame == totalFrames && copy != nullptr) {

		currentFrame = 0;
		animationPreviewPhase = AnimationPhase::COMPLETE;
		fractalRenderer = *copy;
		delete copy;

	}

	if (animationPreviewPhase == AnimationPhase::ANIMATING) {

		if (currentFrame == 0) {
			copy = new FractalRenderer{ 0, 0 };
			*copy = fractalRenderer;
		}


		executeAnimationFunction(fractalRenderer);
		++currentFrame;

	}

}

void Animation::AnimationVideoHandler(FractalRenderer& fractalRenderer) {

	static FractalRenderer* copy;

	static string timeStamp;

	if (currentFrame == totalFrames && copy != nullptr) {

		currentFrame = 0;
		animationVideoPhase = AnimationPhase::COMPLETE;
		fractalRenderer = *copy;
		delete copy;

	}

	if (animationVideoPhase == AnimationPhase::ANIMATING) {

		if (currentFrame == 0) {

			auto now = chrono::system_clock::now();
			auto local_time = chrono::current_zone()->to_local(now);
			timeStamp = format("{:%Y-%m-%d_%H-%M-%S}", local_time);
			filesystem::create_directories("output/video_" + timeStamp + "/");

			copy = new FractalRenderer{ 0, 0 };
			*copy = fractalRenderer;
		}

		string fileName = "video_" + timeStamp + "/" + to_string(currentFrame) + ".png";
		fractalRenderer.renderImage(fileName);

		executeAnimationFunction(fractalRenderer);
		++currentFrame;

	}

}

void Animation::executeAnimationFunction(FractalRenderer& fractalRenderer) {

	switch (animation) {

	case Animations::juliaCircle:
		juliaCircle(fractalRenderer);
		break;

	case Animations::zoom:
		zoom(fractalRenderer);
		break;

	}

}

void Animation::juliaCircle(FractalRenderer& fractalRenderer) {

	const float r = 0.7f;
	float theta = (static_cast<float>(currentFrame) / totalFrames) * 2.0f * numbers::pi;

	fractalRenderer.juliaCx = r * cos(theta);
	fractalRenderer.juliaCy = r * sin(theta);

}

void Animation::zoom(FractalRenderer& fractalRenderer) {
	
	float initalScale = 2.5;
	static float finalScale;
	if (currentFrame == 0) {
		finalScale = fractalRenderer.scale;
		fractalRenderer.scale = initalScale;
	}

	// Sn = S0 * c^n
	// (Scale of frame n = scale inital * constant^frame n)
	float c = pow((finalScale / initalScale), (1.0 / totalFrames));
	fractalRenderer.scale = initalScale * pow(c, currentFrame);

	int initalIteration = 110;
	int finalIteration = 2000;

	c = pow((finalIteration / initalIteration), (1.0 / totalFrames));
	fractalRenderer.maxIterations = initalIteration * pow(c, currentFrame);



}