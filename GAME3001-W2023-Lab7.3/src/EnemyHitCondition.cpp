#include "EnemyHitCondition.h"

EnemyHitCondition::EnemyHitCondition(const bool hit)
{
	m_name = "Enemy Hit Condition";
	SetHit(hit);
}

EnemyHitCondition::~EnemyHitCondition()
= default;

void EnemyHitCondition::SetHit(const bool state)
{
	m_hit = state;
}

bool EnemyHitCondition::Condition()
{
	return m_hit;
}
