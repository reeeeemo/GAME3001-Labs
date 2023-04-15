#pragma once
#ifndef __OBSTACLE__
#define __OBSTACLE__
#include "NavigationObject.h"

class Obstacle : public DisplayObject
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

	[[nodiscard]] bool GetDeleteMe() const;
	void SetDeleteMe(bool deleteMe);
private:
	bool m_deleteMe;
};

#endif /* defined (__OBSTACLE__) */