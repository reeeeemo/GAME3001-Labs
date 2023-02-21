#pragma once
#ifndef __TILE_STATUS__
#define __TILE_STATUS__

enum TileStatus
{
	UNVISITED,
	OPEN,
	CLOSED,
	IMPASSABLE,
	GOAL,
	START,
	PATH
};

#endif /* defined (__TILE_STATUS__) */