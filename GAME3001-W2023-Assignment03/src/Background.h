#pragma once
#pragma once
#ifndef __BACKGROUND__
#define __BACKGROUND__
#include "DisplayObject.h"

class Background final : public DisplayObject
{
public:
	// constructors
	Background();

	// destructor
	~Background();

	// life cycle functions
	void Draw() override;
	void Update() override;
	void Clean() override;
private:

};

#endif /* defined (__OBSTACLE__) */