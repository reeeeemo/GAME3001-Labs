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
	void SpawnEnemy(Enemy* enemyToSpawn, EnemyType type);

	// Getter for vector of enemies
	std::vector<Enemy*> GetPool();
private:
	std::vector<Enemy*> m_pEnemies;
};

#endif //!__ENEMY_POOL_H__