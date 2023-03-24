#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "LOSMode.h"
#include "Obstacle.h"
#include "Scene.h"
#include "Target.h"
#include "StarShip.h"
#include "PathNode.h"

// New for Lab 7.1
#include "DecisionTree.h"
#include "Background.h"
#include "Player.h"
#include "Torpedo.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene() override;

	// Scene LifeCycle Functions
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;
	virtual void HandleEvents() override;
	virtual void Start() override;

private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	glm::vec2 m_mousePosition;
	bool m_isGridEnabled;
	// New for Lab 7.1
	int m_shipPosition[2];
	int m_angle;

	// Game Objects for the Scene
	Target* m_pTarget;
	StarShip* m_pStarShip;
	Player* m_pPlayer;
	TorpedoPool* m_pTorpedoPool;
	std::vector<Obstacle*> m_pObstacles;
	// New for Lab 7.1
	Background* m_pBackground;

	void BuildObstaclePool();

	// Create our Division Scheme (Grid of PathNodes)
	std::vector<PathNode*> m_pGrid;
	void m_buildGrid();
	void m_toggleGrid(bool state) const;
	void m_clearNodes();

	// LOS functions
	bool m_checkAgentLOS(Agent* agent, DisplayObject* target_object) const;
	bool m_checkPathNodeLOS(PathNode* path_node, DisplayObject* target_object) const;
	void m_checkAllNodesWithTarget(DisplayObject* target_object) const;
	void m_checkAllNodesWithBoth() const;
	void m_setPathNodeLOSDistance(int distance) const;

	// LOS Variables (fields)
	LOSMode m_LOSMode{};
	int m_pathNodeLOSDistance;

	// Decision Tree
	DecisionTree* m_decisionTree;
};

#endif /* defined (__PLAY_SCENE__) */