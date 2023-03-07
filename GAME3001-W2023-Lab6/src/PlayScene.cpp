#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include <windows.h>

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

	// Add Game Objects
	m_pTarget = new Target();
	m_pTarget->GetTransform()->position = glm::vec2(600.0f, 300.0f);
	AddChild(m_pTarget);

	m_pStarShip = new StarShip();
	m_pStarShip->GetTransform()->position = glm::vec2(150.0f, 300.0f);
	AddChild(m_pStarShip);

	// Add Obstacles
	BuildObstaclePool();

	m_pObstacles[0]->GetTransform()->position = glm::vec2(380.0f, 80.0f);
	m_pObstacles[0]->SetHeight(50);
	AddChild(m_pObstacles[0]);

	m_pObstacles[1]->GetTransform()->position = glm::vec2(380.0f, 280.0f);
	m_pObstacles[1]->SetWidth(100);
	AddChild(m_pObstacles[1]);

	m_pObstacles[2]->GetTransform()->position = glm::vec2(380.0f, 480.0f);
	AddChild(m_pObstacles[2]);

	// Setup the Grid
	m_isGridEnabled = false;
	m_buildGrid();
	m_toggleGrid(m_isGridEnabled);


	// Preload Sounds

	SoundManager::Instance().Load("../Assets/Audio/yay.ogg", "yay", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/thunder.ogg", "thunder", SoundType::SOUND_SFX);


	ImGuiWindowFrame::Instance().SetGuiFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::GUI_Function()
{
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
	}

	ImGui::Separator();


	ImGui::End();
}

void PlayScene::BuildObstaclePool()
{
	for (int i = 0; i < 3; ++i)
	{
		m_pObstacles.push_back(new Obstacle);
	}
}

void PlayScene::m_buildGrid()
{
	constexpr auto tile_size = Config::TILE_SIZE;

	m_clearNodes(); // We will need to clear nodes because we will rebuild/redraw the grid if we move an obstacle.

	// Lay out a grid of path_nodes
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			PathNode* path_node = new PathNode();
			path_node->GetTransform()->position = glm::vec2(static_cast<float>(col) * tile_size + Config::TILE_OFFSET.x,
				static_cast<float>(row) * tile_size + Config::TILE_OFFSET.y);

			// Only show grid where there arae no obstacles
			bool keep_node = true;
			for (auto obstacle : m_pObstacles)
			{
				// Determine which path_nodes to keep
				if (CollisionManager::AABBCheck(path_node, obstacle));
				{
					keep_node = false;
				}
			}
			if (keep_node)
			{
				AddChild(path_node);
				m_pGrid.push_back(path_node);
			} else
			{
				delete path_node;
			}
		}
	}

	// If grid is supposed to be hidden - make it so!
	m_toggleGrid(m_isGridEnabled);
}

void PlayScene::m_toggleGrid(bool state) const
{
	for (auto path_node : m_pGrid)
	{
		path_node->SetVisible(true);
	}
}

void PlayScene::m_clearNodes()
{
	m_pGrid.clear();
	for (auto object : GetDisplayList())
	{
		if (object->GetType() == GameObjectType::PATH_NODE)
		{
			RemoveChild(object);
		}
	}
}

void PlayScene::m_checkShipLOS(DisplayObject* target_object) const
{
}
