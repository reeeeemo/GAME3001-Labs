#include "Starship.h"
#include "TextureManager.h"

Starship::Starship() {
	TextureManager::Instance().Load("../Assets/textures/ncl.png", "starship");

	const auto size = TextureManager::Instance().GetTextureSize("");
	SetWidth(static_cast<int>(size.x));
	SetHeight(static_cast<int>(size.y));
	GetTransform()->position = glm::vec2(0.0f,0.0f);
	GetRigidBody()->velocity = glm::vec2(0, 0);
	GetRigidBody()->isColliding = false;

	SetType(GameObjectType::STARSHIP);
}

Starship::~Starship()
= default;

void Starship::Draw() {
	TextureManager::Instance().Draw("starship", GetTransform()->position, 0, 255, true);

}

void Starship::Update() {

}

void Starship::Clean() {

}