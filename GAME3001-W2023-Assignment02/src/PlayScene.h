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

	std::vector<Label*> m_pInstructionLabels;
	

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
	void m_buildPathList();
	void m_displayPathList() const;
	void m_resetPathFinding();
	void m_resetSimulation();

	// Tile lists for pathfinding

	std::priority_queue<Tile*, std::vector<Tile*>, TileComparator > m_pOpenList;
	std::list<Tile*> m_pPathList;
	bool m_pathFound = false;

	// Convienience functions to convert world to grid space
	[[nodiscard]] Tile* m_getTile(const int col, const int row) const;
	[[nodiscard]] Tile* m_getTile(const glm::vec2 grid_position) const;

	template <typename T>
	void m_addObjectToGrid(T*& object, int col, int row, TileStatus status);


	template <typename T>
	void m_moveGameObject(T*& object, int col, int row, TileStatus status);

	void m_SetAsObstacle(int columnStart, int rowStart, int columnEnd, int rowEnd) const; // Makes it easier to set chunks of impassable objects at a time.

	void m_moveShipAcrossTilePath();

	// Heuristic
	Heuristic m_currentHeuristic;

	// Tile Map Variables and Functions
	std::string m_tileMap;
	void m_initalizeTileMap();
	void m_buildTileMap();

	// Convenience functions to update the Tile map
	void m_updateTileMap(int col, int row, TileStatus status);
	void m_updateTileMap(glm::vec2 grid_position, TileStatus);
};


#endif /* defined (__PLAY_SCENE__) */
