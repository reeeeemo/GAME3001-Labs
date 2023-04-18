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
	Background(std::string fileName, std::string textureFile);

	// destructor
	~Background();

	// life cycle functions
	void Draw() override;
	void Update() override;
	void Clean() override;
private:
	std::string m_key;
};

#endif /* defined (__OBSTACLE__) */