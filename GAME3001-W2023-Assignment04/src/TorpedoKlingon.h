#pragma once
#ifndef __TORPEDOK_H__
#define __TORPEODK_H__

#include "TorpedoAnimationState.h"
#include "Torpedo.h"

class TorpedoKlingon final : public Torpedo
{
public:
	TorpedoKlingon(float speed, glm::vec2 direction, float damage = 30.0f);

private:
	void BuildAnimations() override;
};

#endif /* defined (__TORPEDOK___) */