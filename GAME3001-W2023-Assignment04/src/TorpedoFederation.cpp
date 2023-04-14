#include "TorpedoFederation.h"

TorpedoFederation::TorpedoFederation(float speed, glm::vec2 direction)
{
	TextureManager::Instance().Load("../Assets/sprites/Player/carrot.png", "carrot");
	TextureManager::Instance().Load("../Assets/textures/Explosion.png", "explosion");

	m_textureKey = "carrot";
	SetSpriteSheet(TextureManager::Instance().GetSpriteSheet(m_textureKey));

	SetWidth(20);
	SetHeight(20);

	m_direction = { direction.x * speed, direction.y * speed };

}

void TorpedoFederation::Draw()
{
	// If we are in debug mode, draw the collider rect.
	if (Game::Instance().GetDebugMode())
	{
		Util::DrawRect(GetTransform()->position -
			glm::vec2(this->GetWidth() * 0.5f, this->GetHeight() * 0.5f),
			this->GetWidth(), this->GetHeight());
	}
	TextureManager::Instance().Draw("carrot", GetTransform()->position, GetTransform()->rotation.r, 255, true, SDL_FLIP_HORIZONTAL);
}