#pragma once
#ifndef __ENEMY_POOL_H__
#define __ENEMY_POOL_H__

#include <vector>

#include "DisplayObject.h"
#include "Agent.h"
#include "EnemyTypes.h"

#include "CloseCombatEnemy.h"
#include "RangedCombatEnemy.h"

class EnemyPool : public DisplayObject
{
public:
	EnemyPool() = default;
	virtual ~EnemyPool() = default;


	void Update() override;
	void Draw() override;
	void Clean() override;

	// Spawn in enemy
	void SpawnEnemy(Agent* enemyToSpawn, EnemyType type);

	// Getter for vector of enemies
	std::vector<Agent*> GetPool();
private:
	std::vector<Agent*> m_pEnemies;
};

#endif //!__ENEMY_POOL_H__