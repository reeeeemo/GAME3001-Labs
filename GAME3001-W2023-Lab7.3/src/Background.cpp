#include "Background.h"
#include "SoundManager.h"
#include "TextureManager.h"

Background::Background()
{
	TextureManager::Instance().Load("../Assets/textures/nebula.png", "background");

	const auto size = TextureManager::Instance().GetTextureSize("background");
	SetWidth(size.x);
	SetHeight(size.y);

	GetTransform()->position = glm::vec2(size.x / 2, size.y / 2);

	SetType(GameObjectType::NONE);
	GetRigidBody()->isColliding = false;
}

Background::~Background()
= default;

void Background::Draw()
{
	TextureManager::Instance().Draw("background",
		GetTransform()->position, 0, 255, true);
}

void Background::Update()
{
}

void Background::Clean()
{
}