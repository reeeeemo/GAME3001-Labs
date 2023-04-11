#include "Torpedo.h"
#include "TextureManager.h"

Torpedo::Torpedo(float speed, glm::vec2 direction): m_currentAnimationState(TorpedoAnimationState::FIRED), m_speed(speed)
{
	TextureManager::Instance().LoadSpriteSheet(
		"../Assets/sprites/torpedo.txt",
		"../Assets/sprites/torpedo.png", 
		"torpedo");

	SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("torpedo"));
	
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

Torpedo::~Torpedo()
= default;

void Torpedo::Draw()
{
	// draw the Torpedo according to animation state
	switch (m_currentAnimationState)
	{
	case TorpedoAnimationState::FIRED:
		TextureManager::Instance().PlayAnimation("torpedo", GetAnimation("fire"),
			GetTransform()->position, 0.25f, 0, 255, true);
		break;
	default:
		break;
	}
}

void Torpedo::Update()
{
	GetTransform()->position.x += m_speed;
}

void Torpedo::Clean()
{
}

void Torpedo::SetAnimationState(const TorpedoAnimationState new_state)
{
	m_currentAnimationState = new_state;
}

void Torpedo::BuildAnimations()
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
