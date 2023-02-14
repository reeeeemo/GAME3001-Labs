#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Target.h"
#include "StarShip.h"
#include "Tile.h"
#include "Heuristic.h"


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

	// Convienience functions to convert world to grid space
	Tile* m_getTile(const int col, const int row) const;
	Tile* m_getTile(const glm::vec2 grid_position) const;
	void m_SetAsObstacle(int columnStart, int rowStart, int columnEnd, int rowEnd); // Makes it easier to set chunks of impassable objects at a time.

	// Heuristic
	Heuristic m_currentHeuristic;
};

#endif /* defined (__PLAY_SCENE__) */