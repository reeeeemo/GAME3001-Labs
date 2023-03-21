#pragma once
#ifndef __CONDITION_NODE__
#define __CONDITION_NODE__
#include "TreeNode.h"

// Abstract Class
class ConditionNode : public TreeNode
{
public:
	ConditionNode() { m_isLeaf = false; }
	virtual ~ConditionNode() = default;

	virtual bool Condition() = 0; // Condition Specialization
};

#endif /* defined (__CONDITION_NODE__) */