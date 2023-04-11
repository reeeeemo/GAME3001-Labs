#include "TorpedoK.h"
#include "TextureManager.h"

TorpedoK::TorpedoK(float speed, glm::vec2 direction): m_currentAnimationState(TorpedoAnimationState::FIRED), m_speed(speed)
{
	TextureManager::Instance().LoadSpriteSheet(
		"../Assets/sprites/torpedo.txt",
		"../Assets/sprites/torpedo_k.png", 
		"TorpedoK");

	SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("TorpedoK"));
	
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

TorpedoK::~TorpedoK()
= default;

void TorpedoK::Draw()
{
	// draw the TorpedoK according to animation state
	switch (m_currentAnimationState)
	{
	case TorpedoAnimationState::FIRED:
		TextureManager::Instance().PlayAnimation("TorpedoK", GetAnimation("fire"),
			GetTransform()->position, 0.25f, 0, 255, true);
		break;
	default:
		break;
	}
}

void TorpedoK::Update()
{
	GetTransform()->position += m_direction;
}

void TorpedoK::Clean()
{
}

void TorpedoK::SetAnimationState(const TorpedoAnimationState new_state)
{
	m_currentAnimationState = new_state;
}

void TorpedoK::BuildAnimations()
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
