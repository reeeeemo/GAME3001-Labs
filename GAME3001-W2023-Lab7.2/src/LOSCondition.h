#pragma once
#ifndef __LOS_CONDITION_H__
#define __LOS_CONDITION_H__

#include "ConditionNode.h"

class LOSCondition : public ConditionNode
{
public:
	LOSCondition(bool hasLOS = false);

	virtual ~LOSCondition();

	// Getters and Setters
	void SetLOS(bool state);

	virtual bool Condition() override;
private:
	bool m_hasLOS;
};

#endif // !__LOS_CONDITION_H__