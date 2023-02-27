#pragma once
#ifndef __STARSHIP__
#define __STARSHIP__

#include "Agent.h"
#include "CowAnimationState.h"
#include "Sprite.h"

class StarShip : public Agent
{
public:
	// Constructor Function
	StarShip();

	// Destructor Function
	~StarShip();

	// LifeCycle Functions
	void Draw() override;
	void Update() override;
	void Clean() override;

	// getters and setters (accessors and mutators)
	float GetMaxSpeed() const;
	float GetTurnRate() const;
	float GetAccelerationRate() const;
	glm::vec2 GetDesiredVelocity() const;

	// Sprite stuff (not exactly convienient, but sprite sheets are kinda fun!)
	SpriteSheet* GetSpriteSheet();
	Animation& GetAnimation(const std::string& name);
	bool CheckIfAnimationExists(const std::string& id);
	void SetSpriteSheet(SpriteSheet* sprite_sheet);
	void SetAnimation(const Animation& animation);
	SpriteSheet* m_pSpriteSheet;
	std::unordered_map<std::string, Animation> m_pAnimations;


	void SetMaxSpeed(float speed);
	void SetTurnRate(float angle);
	void SetAccelerationRate(float rate);
	void SetDesiredVelocity(glm::vec2 target_position);

	// public functions
	void Seek();
	void LookWhereYoureGoing(glm::vec2 target_direction);


private:
	// private movement variables
	float m_maxSpeed;
	float m_turnRate;
	float m_accelerationRate;

	// where we want to go
	glm::vec2 m_desiredVelocity;

	// private function
	void m_move();

	// Sprite functions
	void BuildAnimations();

	// Sprite variables
	CowAnimationState m_currentAnimationState;
};

#endif /* defined (__STARSHIP__)*/

