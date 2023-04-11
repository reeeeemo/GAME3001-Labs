#pragma once
#ifndef __TORPEDO_H__
#define __TORPEOD_H__

#include "TorpedoAnimationState.h"
#include "Sprite.h"

class Torpedo final : public Sprite
{
public:
	Torpedo(float speed = 0.0f, glm::vec2 direction);
	~Torpedo() override;

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

#endif /* defined (__TORPEDO_H__) */