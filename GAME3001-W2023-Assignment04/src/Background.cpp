#include "Background.h"
#include "SoundManager.h"
#include "TextureManager.h"

Background::Background()
{
	m_key = "background";

	TextureManager::Instance().Load("../Assets/sprites/background/Winton.png", m_key);

	const auto size = TextureManager::Instance().GetTextureSize(m_key);
	SetWidth(size.x);
	SetHeight(size.y);

	GetTransform()->position = glm::vec2(size.x / 2, size.y / 2);

	SetType(GameObjectType::NONE);
	GetRigidBody()->isColliding = false;
}

Background::Background(std::string fileName, std::string textureFile)
{
	m_key = fileName;
	TextureManager::Instance().Load(textureFile, fileName);

	const auto size = TextureManager::Instance().GetTextureSize(fileName);
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
	TextureManager::Instance().Draw(m_key,
		GetTransform()->position, 0, 255, true);
}

void Background::Update()
{
}

void Background::Clean()
{
}