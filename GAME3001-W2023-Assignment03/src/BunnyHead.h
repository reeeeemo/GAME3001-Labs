#pragma once
#ifndef __BUNNY_HEAD_H__
#define __BUNNY_HEAD_H__
#include "Sprite.h"


class BunnyHead : public Sprite
{
	void Update() override;
	void Draw() override;
	void Clean() override;
};

#endif //!__BUNNY_HEAD_H__