#pragma once
#ifndef __TILE_H__
#define __TILE_H__

#include "NavigationObject.h"
#include "TileStatus.h"
#include "NeighbourTile.h"
#include "Label.h"

class Tile : public NavigationObject
{
public:
	// Constructor
	Tile();

	// Destructor
	~Tile();

	// Life-Cycle Functions
	void Draw() override;
	void Update() override;
	void Clean() override;

	// Getters (Accessors) and Setters (Mutators)
	[[nodiscard]] Tile* GetNeighbourTile(const NeighbourTile position) const;
	void SetNeighbourTile(NeighbourTile position, Tile* tile);
	std::vector<Tile*> GetNeighbourTiles();

	[[nodiscard]] Tile* GetTileParent() const;
	void SetTileParent(Tile* parent_tile);

	float GetTileCost() const;
	void SetTileCost(float cost);

	[[nodiscard]] TileStatus GetTileStatus() const;
	void SetTileStatus(TileStatus status);

	// Utility Functions
	void AddLabels();
	void SetLabelsEnabled(bool state);

	bool GetIsStart() { return m_isStart;}
	void SetIsStart(bool start) { m_isStart = start;}

	bool GetIsEnd() { return m_isEnd; }
	void SetIsEnd(bool end) { m_isEnd = end; }
	

private:
	// Private instance members (fields)
	float m_cost;
	TileStatus m_status;

	Label* m_costLabel;
	Label* m_statusLabel;

	Tile* m_pTileParent;
	std::vector<Tile*> m_neighbours;

	bool m_isStart;
	bool m_isEnd;
};


#endif //!__TILE_H__
