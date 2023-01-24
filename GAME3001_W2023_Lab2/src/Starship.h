#pragma once
#ifndef __STARSHIP_H__
#define __STARSHIP_H__

#include "Agent.h"
#include "Agent.h"
#include "Agent.h"


class Starship final : public Agent {
public:
	Starship();
	~Starship();

	// LifeCycle Functions
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;

	// getters and setters (accessors and mutators)
	float GetMaxSpeed() const;
	float GetTurnRate();
	float GetAccelerationRate() const;
	glm::vec2 GetDesiredVelocity() const;

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

	// Where we want to go
	glm::vec2 m_desiredVelocity;
	void m_move();
};
#endif // !__STARSHIP_H__
