#include "EnemyPool.h"

void EnemyPool::Update()
{
	for (const auto enemy : m_pEnemies)
	{
		enemy->Update();
	}
}

void EnemyPool::Draw()
{
	for (const auto enemy : m_pEnemies)
	{
		enemy->Draw();
	}
}

void EnemyPool::Clean()
{
	for (auto enemy : m_pEnemies) {
		delete enemy;
		enemy = nullptr;
	}
	m_pEnemies.clear();
	m_pEnemies.shrink_to_fit();
}

void EnemyPool::SpawnEnemy(Agent* enemyToSpawn, EnemyType type)
{
	enemyToSpawn->SetEnemyType(type);
	m_pEnemies.push_back(enemyToSpawn);
}

std::vector<Agent*> EnemyPool::GetPool()
{
	return m_pEnemies;
}
