#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include <fstream>
#include <queue>
#include "Obstacle.h"
#include "Scene.h"
#include "Target.h"
#include "StarShip.h"
#include "PathNode.h"
#include "LOSMode.h"


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

	// Game Objects for the Scene
	Target* m_pTarget;
	StarShip* m_pStarShip;
	std::vector<Obstacle*> m_pObstacles;


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
	LOSMode m_LOSMode{};
	int m_pathNodeLOSDistance;


	// Pathfinding stuff
	void m_getShortestPath();

	std::priority_queue<PathNode*, std::vector<PathNode*>, 

	// Debugging Variables
	bool m_bDebugView;
	bool m_pathFound = false;

};

#endif /* defined (__PLAY_SCENE__) */