#include "Enemy.h"

#include "Game.h"
#include "Util.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
=default;


void Enemy::SetHealth(float health)
{
    m_Health = health;
}

void Enemy::SetSpeed(float speed)
{
    m_Speed = speed;
}

void Enemy::SetMaxSpeed(float maxSpeed)
{
    m_maxSpeed = maxSpeed;
}


void Enemy::SetScoreValue(float scoreValue)
{
    m_scorevalue = scoreValue;
}

void Enemy::SetDeleteMe(bool deleteMe)
{
    m_deleteMe = deleteMe;
}

void Enemy::SetTargetPlayer(GameObject* targetPlayer)
{
    m_pTargetPlayer = targetPlayer;
}


void Enemy::SetAttackDamage(float damage)
{
    m_attackDamage = damage;
}

void Enemy::TakeDamage(float damage)
{
    m_Health-=damage;
    //do something in overloads like flash sprite or play sound
    std::cout << "Enemy took " << damage << " damage." << std::endl;
}

float Enemy::GetSpeed() const
{
    return m_Speed;
}

float Enemy::GetMaxSpeed() const
{
    return m_maxSpeed;
}


float Enemy::GetHealth() const
{
    return m_Health;
}

float Enemy::GetScoreValue() const
{
    return m_scorevalue;
}

bool Enemy::GetDeleteMe() const
{
    return m_deleteMe;
}

GameObject* Enemy::GetTargetPlayer() const
{
    return m_pTargetPlayer;
}


float Enemy::GetAttackDamage()
{
    return m_attackDamage;
}

EnemyPool::EnemyPool()
{ }

void EnemyPool::Update()
{
    for (unsigned i = 0; i < m_pEnemies.size(); i++)
    {
	    if (m_pEnemies[i]->GetDeleteMe() == true)
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

void EnemyPool::Clean()
{
}

void EnemyPool::Draw()
{
    for (Enemy* enemy : m_pEnemies)
    {
        enemy->Draw(); 
    }
}

std::vector<Enemy*> EnemyPool::GetPool()
{
    return m_pEnemies;
}

void EnemyPool::Spawn(Enemy* enemyType)
{
    m_pEnemies.push_back(enemyType);
}

void EnemyPool::UpdateTargetPlayer(GameObject* targetObject) const
{
    for (auto enemy : m_pEnemies)
    {
        enemy->SetTargetPlayer(targetObject);
    }
}

