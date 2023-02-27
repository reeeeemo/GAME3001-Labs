#include "Obstacle.h"


#include "SoundManager.h"
#include "TextureManager.h"

Obstacle::Obstacle()
{
	//TextureManager::Instance().Load("../Assets/textures/mine.png", "obstacle");
	TextureManager::Instance().LoadSpriteSheet("../Assets/sprites/Fences.txt", "../Assets/sprites/Fences.png", "Fences");

	//const auto size = TextureManager::Instance().GetTextureSize("obstacle");
	SetWidth(40);
	SetHeight(40);

	SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("Fences"));
	GetTransform()->position = glm::vec2(300.0f, 300.0f);
	GetRigidBody()->bounds = glm::vec2(GetWidth(), GetHeight());
	setIsCentered(true);

	SetType(GameObjectType::OBSTACLE);
	GetRigidBody()->isColliding = false;
}

Obstacle::~Obstacle()
= default;

void Obstacle::Draw()
{
	TextureManager::Instance().Draw("obstacle", GetTransform()->position, 0, 255, true);
}

void Obstacle::Update()
{
}

void Obstacle::Clean()
{
}

SpriteSheet* Obstacle::GetSpriteSheet()
{
	return m_pSpriteSheet;
}

void Obstacle::SetSpriteSheet(SpriteSheet* sprite_sheet)
{
	m_pSpriteSheet = sprite_sheet;
}
