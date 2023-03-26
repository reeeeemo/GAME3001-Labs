#pragma once
#ifndef __STARSHIP__
#define __STARSHIP__

#include "Agent.h"
#include "Enemy.h"

class StarShip : public Enemy
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
	float GetTurnRate() const;
	float GetAccelerationRate() const;
	glm::vec2 GetDesiredVelocity() const;

	
	void SetTurnRate(float angle);
	void SetAccelerationRate(float rate);
	void SetDesiredVelocity(glm::vec2 target_position);

	// public functions
	void Seek();
	void LookWhereYoureGoing(glm::vec2 target_direction);
	void Reset();

private:
	// private movement variables
	float m_turnRate;
	float m_accelerationRate;
	glm::vec2 m_startPosition;

	// where we want to go
	glm::vec2 m_desiredVelocity;

	// private function
	void m_move();

	// New for Lab 7.1
	std::vector<glm::vec2> m_patrolPath;
	int m_wayPoint;
	void m_buildPatrolPath();
};

#endif /* defined (__STARSHIP__)*/

