#pragma once
#ifndef __DECISION_TREE_H__
#define __DECISION_TREE_H__

#include "Agent.h"
#include "CloseCombatCondition.h"
#include "LOSCondition.h"
#include "RadiusCondition.h"
#include "TreeNode.h"
#include "TreeNodeType.h"
#include "RangedCombatCondition.h"

#include <vector>

class DecisionTree
{
public:
	// Constructor(s)
	DecisionTree();
	DecisionTree(Agent* agent);

	// Destructor(s)
	virtual ~DecisionTree();

	// Accessors and Mutators
	[[nodiscard]] Agent* GetAgent() const;
	void SetAgent(Agent* agent);

	// Condition Node Accessors
	[[nodiscard]] LOSCondition* GetLOSNode() const;
	[[nodiscard]] RadiusCondition* GetRadiusNode() const;
	[[nodiscard]] CloseCombatCondition* GetCloseCombatNode() const;
	[[nodiscard]] RangedCombatCondition* GetRangedCombatNode() const;

	// New for Lab 7.2
	[[nodiscard]] std::vector<TreeNode*>& GetTree();
	void SetLOSNode(LOSCondition* node);
	void SetRadiusNode(RadiusCondition* node);
	void SetCloseCombatNode(CloseCombatCondition* node);
	void SetRangedCombatNode(RangedCombatCondition* node);

	// Public utility methods / convenience methods
	TreeNode* AddNode(TreeNode* parent, TreeNode* child_node, TreeNodeType type);

	// Life Cycle Methods
	void Display() const;
	void Update();
	void Clean();

	// Main public method
	void MakeDecision() const; // In-order traversal

private:
	// private data members (fields)
	Agent* m_agent{};
	LOSCondition* m_LOSNode{};
	RadiusCondition* m_radiusNode{};
	CloseCombatCondition* m_closeCombatNode{};

	// New for Lab 7.2
	RangedCombatCondition* m_rangedCombatNode{};

	// Tree node list
	std::vector<TreeNode*> m_treeNodeList;

	// Private methods
};

#endif //!__DECISION_TREE_H__