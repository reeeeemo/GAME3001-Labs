#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"
#include "Config.h"

PlayScene::PlayScene()
{
	PlayScene::Start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::Draw()
{
	DrawDisplayList();
	
	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	UpdateDisplayList();

}

void PlayScene::Clean()
{
	RemoveAllChildren();
}

void PlayScene::HandleEvents()
{
	EventManager::Instance().Update();

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_ESCAPE))
	{
		Game::Instance().Quit();
	}

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_1))
	{
		Game::Instance().ChangeSceneState(SceneState::START);
	}

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_2))
	{
		Game::Instance().ChangeSceneState(SceneState::END);
	}
}

void PlayScene::Start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";

	m_buildGrid();

	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	// Add the Target to the Scene
	m_pTarget = new Target(); // instantiate an object of type Target
	m_pTarget->GetTransform()->position = m_getTile(15, 11)->GetTransform()->position + offset;
	m_pTarget->SetGridPosition(15.0f, 11.0f);
	m_getTile(15, 11)->SetTileStatus(GOAL);
	AddChild(m_pTarget, 1);

	// Add the StarShip to the Scene
	m_pStarShip = new StarShip();
	m_pStarShip->GetTransform()->position = m_getTile(1, 3)->GetTransform()->position + offset;
	m_pStarShip->SetGridPosition(1.0, 3.0f);
	m_getTile(1, 3)->SetTileStatus(START);
	m_pStarShip->SetTargetPosition(m_pTarget->GetTransform()->position);
	m_pStarShip->SetCurrentDirection(glm::vec2(1.0f, 0.0f)); // facing right
	AddChild(m_pStarShip, 2);

	// Preload Sounds

	SoundManager::Instance().Load("../Assets/Audio/yay.ogg", "yay", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/thunder.ogg", "thunder", SoundType::SOUND_SFX);

	ImGuiWindowFrame::Instance().SetGuiFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::GUI_Function()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("GAME3001 - W2023 - Lab4", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar );

	ImGui::Separator();

	// Debug Properties
	static bool toggleGrid = false;
	if(ImGui::Checkbox("Toggle Grid", &toggleGrid))
	{
		m_isGridEnabled = toggleGrid;
		m_setGridEnabled(m_isGridEnabled);
	}

	ImGui::Separator();
	// Starship properties

	static int start_position[2] = { static_cast<int>(m_pStarShip->GetGridPosition().x),static_cast<int>(m_pStarShip->GetGridPosition().y) };
	if (ImGui::SliderInt2("Start Position", start_position, 0, Config::COL_NUM - 1))
	{
		// Constrain the object within max rows
		if (start_position[1] > Config::ROW_NUM - 1)
		{
			start_position[1] = Config::ROW_NUM - 1;
		}

		// Convert grid space to world space
		m_getTile(m_pStarShip->GetGridPosition())->SetTileStatus(UNVISITED);
		m_pStarShip->GetTransform()->position = m_getTile(start_position[0], start_position[1])->GetTransform()->position + offset;
		m_pStarShip->SetGridPosition(start_position[0], start_position[1]);
		m_getTile(m_pStarShip->GetGridPosition())->SetTileStatus(START);
	}
	ImGui::Separator();

	// Target Properties
		static int goal_position[2] = { static_cast<int>(m_pTarget->GetGridPosition().x),static_cast<int>(m_pTarget->GetGridPosition().y) };
	if (ImGui::SliderInt2("Goal Position", goal_position, 0, Config::COL_NUM - 1))
	{
		// Constrain the object within max rows
		if (goal_position[1] > Config::ROW_NUM - 1)
		{
			goal_position[1] = Config::ROW_NUM - 1;
		}

		// Convert grid space to world space
		m_getTile(m_pTarget->GetGridPosition())->SetTileStatus(UNVISITED);
		m_pTarget->GetTransform()->position = m_getTile(goal_position[0], goal_position[1])->GetTransform()->position + offset;
		m_pTarget->SetGridPosition(goal_position[0], goal_position[1]);
		m_getTile(m_pTarget->GetGridPosition())->SetTileStatus(GOAL);
	}
	ImGui::End();
}

void PlayScene::m_buildGrid()
{
	const auto tile_size = Config::TILE_SIZE;

	// Layout a grid of tiles
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = new Tile();
			// World space position
			tile->GetTransform()->position = glm::vec2(col * tile_size, row * tile_size);
			tile->SetGridPosition(col, row); // Grid Position
			tile->SetParent(this);
			tile->AddLabels();
			AddChild(tile);
			tile->SetEnabled(false);
			m_pGrid.push_back(tile);
		}

	}

	// setup the neighbour references for each tile in the grid
	// tiles = nodes in our graph

	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = m_getTile(col, row);
			// TopMost Neighbour
			if (row == 0)
			{
				tile->SetNeighbourTile(TOP_TILE, nullptr);
			} else
			{
				tile->SetNeighbourTile(TOP_TILE, m_getTile(col, row -1));
			}

			// RightMost Neighbour
			if (col == Config::COL_NUM - 1)
			{
				tile->SetNeighbourTile(RIGHT_TILE, nullptr);

			} else
			{
				tile->SetNeighbourTile(RIGHT_TILE, m_getTile(col + 1, row));

			}

			// BottomMost Neighbour
			if (row == Config::ROW_NUM - 1)
			{
				tile->SetNeighbourTile(BOTTOM_TILE, nullptr);

			}
			else
			{
				tile->SetNeighbourTile(BOTTOM_TILE, m_getTile(col, row + 1));

			}

			// LeftMost Neighbour
			if (col == 0)
			{
				tile->SetNeighbourTile(LEFT_TILE, nullptr);

			}
			else
			{
				tile->SetNeighbourTile(LEFT_TILE, m_getTile(col - 1, row));

			}

		}
	}

}

bool PlayScene::m_getGridEnabled() const
{
	return m_isGridEnabled;
}

void PlayScene::m_setGridEnabled(bool state)
{
	m_isGridEnabled = state;

	for (auto tile : m_pGrid)
	{
		tile->SetEnabled(m_isGridEnabled); // Enables each tile object
		tile->SetLabelsEnabled(m_isGridEnabled); // Enables the corresponding labels
	}
}

void PlayScene::m_computeTileCosts()
{
	// For next lab part	
}

Tile* PlayScene::m_getTile(int col, int row) const
{
	return m_pGrid[(row * Config::COL_NUM) + col];
}

Tile* PlayScene::m_getTile(glm::vec2 grid_position) const
{
	const auto col = grid_position.x;
	const auto row = grid_position.y;

	return m_getTile(col, row);
}
