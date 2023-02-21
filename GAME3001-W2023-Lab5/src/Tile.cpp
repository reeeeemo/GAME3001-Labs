#include "Tile.h"

#include "Util.h"
#include "Config.h"

#include <sstream>
#include <iomanip>

Tile::Tile() : m_cost(0.0f)
{
	SetWidth(Config::TILE_SIZE);
	SetHeight(Config::TILE_SIZE);
}

Tile::~Tile()
= default;

void Tile::Draw()
{
	switch(m_status)
	{
	case TileStatus::START:
		Util::DrawFilledRect(GetTransform()->position, GetWidth(), GetHeight(), glm::vec4(0.5f, 1.0f,0.5f,1.0f));
		break;
	case TileStatus::GOAL:
		Util::DrawFilledRect(GetTransform()->position, GetWidth(), GetHeight(), glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));
		break;
	case TileStatus::IMPASSABLE:
		Util::DrawFilledRect(GetTransform()->position, GetWidth(), GetHeight(), glm::vec4(0.5, 0.5, 0.5, 1.0f));
		break;
	case TileStatus::PATH:
		Util::DrawFilledRect(GetTransform()->position, GetWidth(), GetHeight(), glm::vec4(0.0f, 1.0f, 0.0f, 0.5f));
		break;
	case TileStatus::CLOSED:
		Util::DrawFilledRect(GetTransform()->position, GetWidth(), GetHeight(), glm::vec4(0.1f, 0.1f, 0.1f, 0.5f));
		break;
	case TileStatus::OPEN:
		Util::DrawFilledRect(GetTransform()->position, GetWidth(), GetHeight(), glm::vec4(1.0f, 0.0f, 0.5f, 0.5f));
		break;
	default:
		Util::DrawRect(GetTransform()->position, GetWidth(), GetHeight());
		break;
	}
}

void Tile::Update()
{
}

void Tile::Clean()
{
}

Tile* Tile::GetNeighbourTile(const NeighbourTile position) const
{
	return m_neighbours[static_cast<int>(position)];
}

void Tile::SetNeighbourTile(NeighbourTile position, Tile* tile)
{
	m_neighbours[static_cast<int>(position)] = tile;
}

Tile* Tile::GetTileParent() const
{
	return m_pTileParent;
}

void Tile::SetTileParent(Tile* parent_tile)
{
	m_pTileParent = parent_tile;
}

float Tile::GetTileCost() const
{
	return m_cost;
}

void Tile::SetTileCost(float cost)
{
	m_cost = cost;

	// convert string format to single precision and set the label
	std::stringstream stream;
	stream << std::fixed << std::setprecision(1) << m_cost;
	const std::string cost_string = stream.str();

	m_costLabel->SetText(cost_string);
}

TileStatus Tile::GetTileStatus() const
{
	return m_status;
}

void Tile::SetTileStatus(TileStatus status)
{
	m_status = status;

	switch(m_status)
	{
	case TileStatus::UNVISITED:
		m_statusLabel->SetText("-");
		break;
	case TileStatus::OPEN:
		m_statusLabel->SetText("O");
		break;
	case TileStatus::CLOSED:
		m_statusLabel->SetText("C");
		break;
	case TileStatus::IMPASSABLE:
		m_statusLabel->SetText("I");
		break;
	case TileStatus::GOAL:
		m_statusLabel->SetText("G");
		break;
	case TileStatus::START:
		m_statusLabel->SetText("S");
		break;
	}
}

void Tile::AddLabels()
{

	// Cost Label
	m_costLabel = new Label("99.9", "Consolas", 12);
	m_costLabel->GetTransform()->position = GetTransform()->position + Config::TILE_OFFSET + glm::vec2(0.0f, -6.0f);
	GetParent()->AddChild(m_costLabel);
	m_costLabel->SetEnabled(false);
	// Status Label
	m_statusLabel = new Label("-", "Consolas", 12);
	m_statusLabel->GetTransform()->position = GetTransform()->position + Config::TILE_OFFSET + glm::vec2(0.0f, 6.0f);
	GetParent()->AddChild(m_statusLabel);
	m_statusLabel->SetEnabled(false);

}

void Tile::SetLabelsEnabled(bool state)
{
	m_costLabel->SetEnabled(state);
	m_statusLabel->SetEnabled(state);
}
