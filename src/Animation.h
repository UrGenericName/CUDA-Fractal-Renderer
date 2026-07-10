#pragma once

#include "FractalRenderer.h"

class Animation {
public:

	enum class AnimationPhase {

		ANIMATING,
		COMPLETE

	};
	
	enum class Animations {

		null,
		juliaCircle,
		zoom

	};

	Animations animation = Animations::null;
	AnimationPhase animationVideoPhase = AnimationPhase::COMPLETE;
	AnimationPhase animationPreviewPhase = AnimationPhase::COMPLETE;

	int currentFrame = 0;
	int totalFrames = 50;

	void Animate(FractalRenderer& fractalRenderer);

private:

	void AnimationPreviewHandler(FractalRenderer& fractalRenderer);
	void AnimationVideoHandler(FractalRenderer& fractalRenderer);

	void executeAnimationFunction(FractalRenderer& fractalRenderer);

	void juliaCircle(FractalRenderer& fractalRenderer);
	void zoom(FractalRenderer& fractalRenderer);

};