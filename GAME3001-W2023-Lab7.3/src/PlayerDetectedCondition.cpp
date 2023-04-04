#include "PlayerDetectedCondition.h"

PlayerDetectedCondition::PlayerDetectedCondition(const bool playerDetected)
{
	m_name = "Player Detected Condition";
	SetPlayerDetected(playerDetected);
}

PlayerDetectedCondition::~PlayerDetectedCondition()
= default;

void PlayerDetectedCondition::SetPlayerDetected(const bool state)
{
	isDetected = state;
}

bool PlayerDetectedCondition::Condition()
{
	return isDetected;
}
