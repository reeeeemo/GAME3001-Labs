#include "Starship.h"
#include "TextureManager.h"


Starship::Starship() {
	TextureManager::Instance().Load("");

	const auto size = TextureManager::Instance().GetTextureSize("circle");
}