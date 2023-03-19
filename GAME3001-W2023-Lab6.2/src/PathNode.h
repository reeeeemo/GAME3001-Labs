#pragma once
#ifndef __PATH_NODE__
#define __PATH_NODE__

#include "Agent.h"

class PathNode : public Agent
{
public:
	// Constructor
	PathNode();

	// Destructor
	~PathNode();

	// Inherited via GameObject
	void Draw() override;
	void Update() override;
	void Clean() override;
	

	bool m_isTarget = false;
	bool m_isPlayer = false;
};

#endif /* defined (__PATH_NODE__) */