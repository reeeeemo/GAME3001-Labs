#pragma once
#ifndef __PLACEHOLDER_SPRITE_H__
#define __PLACEHOLDER_SPRITE_H__
#include "Sprite.h"


class PlaceholderSprite : public Sprite
{
	void Update() override;
	void Draw() override;
	void Clean() override;
};

#endif //!__PLACEHOLER_SPRITE_H__