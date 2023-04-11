#pragma once
#ifndef __TORPEDOK_H__
#define __TORPEODK_H__

#include "TorpedoAnimationState.h"
#include "Sprite.h"

class TorpedoK final : public Sprite
{
public:
	TorpedoK(float speed = 0.0f);
	~TorpedoK() override;

	// Life Cycle Methods
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;

	// setters
	void SetAnimationState(TorpedoAnimationState new_state);

private:
	void BuildAnimations();

	TorpedoAnimationState m_currentAnimationState;

	float m_speed;
	glm::vec2 m_direction;
};

#endif /* defined (__TORPEDOK___) */