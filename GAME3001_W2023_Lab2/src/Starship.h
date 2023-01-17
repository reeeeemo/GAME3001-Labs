#pragma once
#ifndef __STARSHIP_H__
#define __STARSHIP_H__

#include "DisplayObject.h"

class Starship final : public DisplayObject {
public:
	Starship();
	~Starship();

	// Inherited via GameObject
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;

private:
	void Move();
	void CheckBounds();
	void Reset();
};
#endif // !__STARSHIP_H__
