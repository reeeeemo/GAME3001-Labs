#include "EnemyPool.h"

void EnemyPool::Update()
{
	for (int i = 0; i < m_pEnemies.size(); i++)
	{
		if (m_pEnemies[i]->GetHealth() <= 0)
		{
			m_pEnemies[i]->SetDeleteMe(true);
		}
		if (m_pEnemies[i]->GetDeleteMe())
		{
			delete m_pEnemies[i];
			m_pEnemies[i] = nullptr;
			m_pEnemies.erase(i + m_pEnemies.begin());
			m_pEnemies.shrink_to_fit();
		} else
		{
			m_pEnemies[i]->Update();
		}
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
