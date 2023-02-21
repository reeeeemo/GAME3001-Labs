#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include <queue>

#include "Scene.h"
#include "Target.h"
#include "StarShip.h"
#include "Tile.h"
#include "Heuristic.h"
#include "TileComparator.h"
#include "Obstacle.h"


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

	// Game Objects for the Scene
	Target* m_pTarget;
	StarShip* m_pStarShip;
	

	// Debugging Variables
	bool m_bDebugView;

	// Pathfinding Objects and Functions
	std::vector<Tile*> m_pGrid;
	bool m_isGridEnabled;

	void m_buildGrid();
	bool m_getGridEnabled() const;
	void m_setGridEnabled(bool state);
	void m_computeTileCosts();

	// Obstacle list
	std::vector<Obstacle*> m_pObstacles;
	void m_pBuildObstacles();

	// Obstacle Utility functions.
	void m_removeObstacleAt(int col, int row);
	void m_removeObstacleAt(glm::vec2 grid_position);
	void m_removeAllObstacles();

	// Calculate shortest path functions
	void m_findShortestPath();
	void m_displayPathList();
	void m_resetPathFinding();
	void m_resetSimulation();

	// Tile lists for pathfinding
	std::vector<Tile*> m_pOpenList;
	std::vector<Tile*> m_pClosedList;
	// TODO: some kind of data structure for the path list.
	// Suggestions: Queue data structure or a linked list.

	std::priority_queue<Tile*, std::vector<Tile*>, TileComparator > m_pPathList;

	// Convienience functions to convert world to grid space
	Tile* m_getTile(const int col, const int row) const;
	Tile* m_getTile(const glm::vec2 grid_position) const;

	template <typename T>
	void m_addObjectToGrid(T*& object, int col, int row, TileStatus status);


	template <typename T>
	void m_moveGameObject(T*& object, int col, int row, TileStatus status);

	void m_SetAsObstacle(int columnStart, int rowStart, int columnEnd, int rowEnd) const; // Makes it easier to set chunks of impassable objects at a time.

	// Heuristic
	Heuristic m_currentHeuristic;
};


#endif /* defined (__PLAY_SCENE__) */
