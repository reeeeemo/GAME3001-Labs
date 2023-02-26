#pragma once
#ifndef __TILE_COMPARATOR_H__
#define __TILE_COMPARATOR_H__

#include "Tile.h"

struct TileComparator
{
	int operator() (const Tile* t1, const Tile* t2) const
	{
		return t1->GetTileCost() > t2->GetTileCost();
	}
};


#endif // !__TILE_COMPARATOR_H__