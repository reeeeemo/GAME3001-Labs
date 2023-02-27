#include "Target.h"
#include "TextureManager.h"


Target::Target()
{
	TextureManager::Instance().Load("../Assets/textures/fruit.png","fruit");

	const auto size = TextureManager::Instance().GetTextureSize("fruit");
	SetWidth(static_cast<int>(size.x));
	SetHeight(static_cast<int>(size.y));
	GetTransform()->position = glm::vec2(100.0f, 100.0f);
	GetRigidBody()->velocity = glm::vec2(0, 0);
	GetRigidBody()->isColliding = false;

	SetType(GameObjectType::TARGET);
}

Target::~Target()
= default;

void Target::Draw()
{
	// draw the target
	TextureManager::Instance().Draw("fruit", GetTransform()->position, 0, 255, true);
}

void Target::Update()
{
	Move();
	CheckBounds();
}

void Target::Clean()
{
}

void Target::Move()
{
	GetTransform()->position = GetTransform()->position + GetRigidBody()->velocity * 5.0f;
}

void Target::CheckBounds()
{
}

void Target::Reset()
{
}
