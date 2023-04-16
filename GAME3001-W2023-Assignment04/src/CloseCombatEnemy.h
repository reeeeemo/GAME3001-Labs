#pragma once
#ifndef __CLOSE_COMBAT_ENEMY_H__
#define __CLOSE_COMBAT_ENEMY_H__

#include "Agent.h"
#include "DecisionTree.h"
#include "AttackAction.h"
#include "Enemy.h"
#include "WaitBehindCoverAction.h"
#include "MoveToCoverAction.h"
#include "FleeAction.h"
#include "Sprite.h"
#include "PlaceholderSprite.h"

class CloseCombatEnemy : public Enemy
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
	void Attack() override;

private:
	void BuildAnimations() override;
	//void m_move() override;
	void m_buildTree() override;
	float timerUntilHit;
};



#endif /* defined (__CLOSE_COMBAT_ENEMY_H__)*/

