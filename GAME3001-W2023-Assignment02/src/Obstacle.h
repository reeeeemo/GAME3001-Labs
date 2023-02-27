#pragma once
#ifndef __OBSTACLE__
#define __OBSTACLE__
#include "NavigationObject.h"
#include "SpriteSheet.h"

class Obstacle final : public NavigationObject
{
public:
	// constructors
	Obstacle();
	
	// destructor
	~Obstacle() override;
	
	// life cycle functions
	void Draw() override;
	void Update() override;
	void Clean() override;
	SpriteSheet* GetSpriteSheet();
	void SetSpriteSheet(SpriteSheet* sprite_sheet);
private:
	SpriteSheet* m_pSpriteSheet;
	
};

#endif /* defined (__OBSTACLE__) */