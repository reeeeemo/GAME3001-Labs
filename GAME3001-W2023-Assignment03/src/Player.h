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
	void MeleeAttack();


	// setters
	void SetAnimationState(PlayerAnimationState new_state);
	void SetHealth(float health);
	void TakeDamage(float dmg);
	void SetRangeOfAttack(float value);



	// Getters
	float GetHealth() const;
	[[nodiscard]] float GetRangeOfAttack();

private:
	void BuildAnimations();
	PlayerAnimationState m_currentAnimationState;
	void m_LookAtMouse();
	float m_Health;
	float m_rangeOfAttack;
};

#endif /* defined (__PLAYER__) */