#pragma once
#ifndef __PLAYER_DETECTED_CONDITION_H__
#define __PLAYER_DETECTED_CONDITION_H__

#include "ConditionNode.h"

class PlayerDetectedCondition : public ConditionNode
{
public:
	PlayerDetectedCondition(bool playerDetected = false);

	virtual ~PlayerDetectedCondition();

	// Getters and Setters
	void SetPlayerDetected(bool state);

	virtual bool Condition() override;
private:
	bool isDetected;
};

#endif // !__PLAYER_DETECTED_CONDITION_H__