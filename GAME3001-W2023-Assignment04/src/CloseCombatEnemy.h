#pragma once
#ifndef __CLOSE_COMBAT_ENEMY_H__
#define __CLOSE_COMBAT_ENEMY_H__

#include "Agent.h"
#include "DecisionTree.h"
#include "AttackAction.h"
#include "WaitBehindCoverAction.h"
#include "MoveToCoverAction.h"
#include "FleeAction.h"
#include "Sprite.h"
#include "PlaceholderSprite.h"

class CloseCombatEnemy : public Agent
{
public:
	// Constructor Function
	CloseCombatEnemy(Scene* scene);

	// Destructor Function
	~CloseCombatEnemy();

	// LifeCycle Functions
	void Draw() override;
	void Update() override;
	void Clean() override;

	// getters and setters (accessors and mutators)
	[[nodiscard]] float GetMaxSpeed() const;
	[[nodiscard]] float GetTurnRate() const;
	[[nodiscard]] float GetAccelerationRate() const;
	[[nodiscard]] glm::vec2 GetDesiredVelocity() const;
	[[nodiscard]] float GetMaxRange() const;
	[[nodiscard]] float GetMinRange() const;


	void SetMaxSpeed(float speed);
	void SetTurnRate(float angle);
	void SetAccelerationRate(float rate);
	void SetDesiredVelocity(glm::vec2 target_position);

	// public functions
	void Seek();
	void LookWhereYoureGoing(glm::vec2 target_direction, bool direction_hack);
	void Reset();

	// New for Lab 7.2
	void Patrol() override;
	void MoveToLOS() override;
	void MoveToPlayer() override;
	void Attack() override;
	DecisionTree* GetTree() const;


private:
	void BuildAnimations();
	// private movement variables
	float m_maxSpeed;
	float m_turnRate;
	float last_rotation;
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

	// New for Lab 7.2
	void m_buildTree();
	DecisionTree* m_tree;

	// New for Lab 8
	Scene* m_pScene; // PlayScene
	bool m_movingTowardsPlayer = false;
	Sprite* m_sprite;

	float maxRangeDistance;
	float minRangeDistance;
};

#endif /* defined (__CLOSE_COMBAT_ENEMY_H__)*/

