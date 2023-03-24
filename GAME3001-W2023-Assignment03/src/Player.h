#pragma once
#ifndef __PLAYER__
#define __PLAYER__

#include "PlayerAnimationState.h"
#include "Sprite.h"

class Player final : public Sprite
{
public:
	Player();
	~Player() override;

	// Life Cycle Methods
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;
	void Move();
	

	// setters
	void SetAnimationState(PlayerAnimationState new_state);
	void SetHealth(float health);
	float GetHealth() const;
	void TakeDamage(float dmg);

private:
	void BuildAnimations();
	PlayerAnimationState m_currentAnimationState;
	void m_LookAtMouse();
	float m_Health;
};

#endif /* defined (__PLAYER__) */