#pragma once
#ifndef __RANGED_COMBAT_ENEMY_H__
#define __RANGED_COMBAT_ENEMY_H__

#include "Agent.h"
#include "DecisionTree.h"
#include "AttackAction.h"
#include "Enemy.h"
#include "Sprite.h"

class RangedCombatEnemy : public Enemy
{
public:
	// Constructor Function
	RangedCombatEnemy(Scene* scene);

	// Destructor Function
	~RangedCombatEnemy();

	// LifeCycle Functions
	void Draw() override;
	void Update() override;
	void Clean() override;
	void Attack() override;
private:
	void BuildAnimations() override;
	//void m_move() override;
	void m_buildTree() override;
	void m_buildLeftTree();
	void m_buildRightTree();

	// New for Lab 8.
	int m_fireCounter; // Number of frames that have elapsed
	int m_fireCounterMax; // Frame delay
	
};

#endif /* defined (__RANGED_COMBAT_ENEMY_H__)*/

