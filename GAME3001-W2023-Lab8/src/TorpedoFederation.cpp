#include "TorpedoFederation.h"

TorpedoFederation::TorpedoFederation(float speed, glm::vec2 direction)
{
	TextureManager::Instance().LoadSpriteSheet(
		"../Assets/sprites/torpedo.txt",
		"../Assets/sprites/torpedo_k.png",
		"torpedo");
	m_textureKey = "torpedo";
	SetSpriteSheet(TextureManager::Instance().GetSpriteSheet(m_textureKey));

	// set frame width
	SetWidth(64);

	// set frame height
	SetHeight(64);

	GetTransform()->position = glm::vec2(400.0f, 300.0f);
	GetRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->isColliding = false;
	SetType(GameObjectType::PROJECTILE);

	m_direction = { direction.x * speed, direction.y * speed };

	BuildAnimations();
}

void TorpedoFederation::BuildAnimations()
{
	auto fire_animation = Animation();

	fire_animation.name = "fire";
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired1"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired2"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired3"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired4"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired5"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired6"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired7"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired8"));

	SetAnimation(fire_animation);
}
