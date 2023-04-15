#include "Obstacle.h"


#include "SoundManager.h"
#include "TextureManager.h"

Obstacle::Obstacle()
{
	TextureManager::Instance().Load("../Assets/sprites/background/rock.png", "obstacle");

	const auto size = TextureManager::Instance().GetTextureSize("obstacle");
	SetWidth(static_cast<int>(size.x));
	SetHeight(static_cast<int>(size.y));

	GetTransform()->position = glm::vec2(400.0f, 300.0f);
	//GetRigidBody()->bounds = glm::vec2(GetWidth(), GetHeight());
	//setIsCentered(true);

	SetType(GameObjectType::OBSTACLE);
	GetRigidBody()->isColliding = false;
}

Obstacle::~Obstacle()
= default;

void Obstacle::Draw()
{
	TextureManager::Instance().Draw("obstacle", GetTransform()->position, this, 0, 255, true);
}

void Obstacle::Update()
{
}

void Obstacle::Clean()
{
}

bool Obstacle::GetDeleteMe() const
{
	return m_deleteMe;
}

void Obstacle::SetDeleteMe(bool deleteMe)
{
	m_deleteMe = deleteMe;
}