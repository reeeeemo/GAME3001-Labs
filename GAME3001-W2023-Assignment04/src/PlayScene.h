#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "LOSMode.h"
#include "Obstacle.h"
#include "Scene.h"
#include "Target.h"
#include "StarShip.h"
#include "PathNode.h"
#include "Label.h"
#include "Player.h"

// New for Lab 7.1
#include "Background.h"

// New for Lab 7.2
#include "CloseCombatEnemy.h"
#include "RangedCombatEnemy.h"
#include "Torpedo.h"
#include "TorpedoKlingon.h"
#include "TorpedoFederation.h"


// New for Assignment 4
#include "EnemyPool.h"

//#define CLOSE_COMBAT

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

	// New for Lab 8.
	void SpawnEnemyTorpedo(Agent* enemyShooting);
	[[nodiscard]] Player* GetTarget() const;
private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	glm::vec2 m_mousePosition;
	bool m_isGridEnabled;
	// New for Lab 7.1
	int m_shipPosition[2];
	int m_angle;
	void CheckCollision();

	// Game Objects for the Scene
	Player* m_pPlayer;
	//StarShip* m_pStarShip;
	std::vector<Obstacle*> m_pObstacles;
	// New for Lab 7.1
	Background* m_pBackground;
	
	float starShipRadius = 300.0f;
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

	// Debugging Variables
	bool m_bDebugView;

	// New for Assignment 4
	EnemyPool* m_pEnemyPool;
	TorpedoPool* m_pTorpedoPool;
	Label* m_RemainingEnemiesLabel;
};

#endif /* defined (__PLAY_SCENE__) */