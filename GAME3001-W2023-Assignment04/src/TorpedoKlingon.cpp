#include "TorpedoKlingon.h"
#include "TextureManager.h"

TorpedoKlingon::TorpedoKlingon(float speed = 0.0f, glm::vec2 direction = { 0.0, 0.0 }, float damage)
{
	// Variable initialization.
	m_speed = speed;

	m_textureKey = "projectile";


	TextureManager::Instance().Load("../Assets/sprites/Player/projectile.png", m_textureKey);

	SetSpriteSheet(TextureManager::Instance().GetSpriteSheet(m_textureKey));
	SetDamage(damage);
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
	
}


//void TorpedoKlingon::Draw()
//{
//	 draw the TorpedoKlingon according to animation state
//	switch (m_currentAnimationState)
//	{
//	case TorpedoAnimationState::FIRED:
//		TextureManager::Instance().PlayAnimation("TorpedoKlingon", GetAnimation("fire"),
//			GetTransform()->position, 0.25f, 0, 255, true);
//		break;
//	default:
//		break;
//	}
//}

