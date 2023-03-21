#pragma once
#ifndef __DECISION_TREE_H__
#define __DECISION_TREE_H__

#include "Agent.h"
#include "CloseCombatCondition.h"
#include "LOSCondition.h"
#include "RadiusCondition.h"
#include "TreeNode.h"
#include "TreeNodeType.h"

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
	Agent* GetAgent() const;
	void SetAgent(Agent* agent);

	// Condition Node Accessors
	LOSCondition* GetLOSNode() const;
	RadiusCondition* GetRadiusNode() const;
	CloseCombatCondition* GetCloseCombatNode() const;

	// Public utility methods / convenience methods
	TreeNode* AddNode(TreeNode* parent, TreeNode* child_node, TreeNodeType type);

	// Life Cycle Methods
	void Display() const;
	void Update();
	void Clean();

private:
	// private data members (fields)
	Agent* m_agent{};
	LOSCondition* m_LOSNode{};
	RadiusCondition* m_radiusNode{};
	CloseCombatCondition* m_closeCombatNode{};

	// Tree node list
	std::vector<TreeNode*> m_treeNodeList;

	// Private methods
	void m_buildTree();

};

#endif //!__DECISION_TREE_H__