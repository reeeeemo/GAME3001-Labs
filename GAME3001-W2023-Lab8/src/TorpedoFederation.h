#pragma once
#ifndef __TORPEDO_FEDERATION_H__
#define __TORPEDO_FEDERATION_H__

#include "Torpedo.h"
#include "TextureManager.h"

class TorpedoFederation final : public Torpedo
{
public:
	TorpedoFederation(float speed, glm::vec2 direction);
private:
	void BuildAnimations() override;

};

#endif //!__TORPEDO_FEDERATION_H__