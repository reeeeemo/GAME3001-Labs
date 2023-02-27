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

	if (!m_pPathList.empty())
	{
		m_moveShipAcrossTilePath();
	}
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
	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_F))
	{
		if (!m_pPathList.empty())
		{
			m_resetPathFinding();
		}
		m_findShortestPath();
	}
	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_R))
	{
		m_resetSimulation();
	}
	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_M))
	{
		if (!m_pPathList.empty())
		{
			m_moveShipAcrossTilePath();
		}
	}
	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_H))
	{
		if (!debugHasBeenPressed)
		{
			m_SetDebugEnabled(!m_getDebugEnabled());
			debugHasBeenPressed = true;
		}
	} else
	{
		debugHasBeenPressed = false;
	}
	if(m_getDebugEnabled())
	{
		if (EventManager::Instance().MousePressed(1)) //start
		{
			SDL_GetMouseState(&m_mousePosition.x, &m_mousePosition.y);
			if (!m_getTile(m_mousePosition.x / Config::TILE_SIZE, m_mousePosition.y / Config::TILE_SIZE)->GetIsEnd())
			{
				m_moveGameObject(m_pStarShip, m_mousePosition.x / Config::TILE_SIZE, m_mousePosition.y / Config::TILE_SIZE, TileStatus::START);
				m_computeTileCosts();
				m_findShortestPath();
			}
			else
			{
				std::cout << "IS GOAL" << std::endl;
			}
				

		}
		if (EventManager::Instance().MousePressed(3)) //goal
		{
			SDL_GetMouseState(&m_mousePosition.x, &m_mousePosition.y);
			if (!m_getTile(m_mousePosition.x / Config::TILE_SIZE, m_mousePosition.y / Config::TILE_SIZE)->GetIsStart())
			{
				m_moveGameObject(m_pTarget, m_mousePosition.x / Config::TILE_SIZE, m_mousePosition.y / Config::TILE_SIZE, TileStatus::GOAL);
				m_computeTileCosts();
				m_findShortestPath();
			}
			else
			{
				std::cout << "IS START" << std::endl;
			}
		}
	}
	
}

void PlayScene::Start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";

	m_pBuildObstacles();
	m_buildGrid();
	m_currentHeuristic = Heuristic::MANHATTAN;

	// Build Tile Map
	m_initalizeTileMap();
	m_buildTileMap();

	// Update path-finding costs
	m_computeTileCosts();


	m_findShortestPath();

	// Preload Sounds

	SoundManager::Instance().Load("../Assets/Audio/yay.ogg", "yay", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/thunder.ogg", "thunder", SoundType::SOUND_SFX);

	const SDL_Color blue = { 0, 0, 255, 255 };
	m_pInstructionLabels.push_back(new Label("F to find shortest path", "Consolas", 18, blue, glm::vec2(120.0f, 20.0f)));
	m_pInstructionLabels.push_back(new Label("R to reset scene", "Consolas", 18, blue, glm::vec2(120.0f, 40.0f)));
	m_pInstructionLabels.push_back(new Label("M to move to goal position", "Consolas", 18, blue, glm::vec2(120.0f, 60.0f)));
	m_pInstructionLabels.push_back(new Label("H to toggle debug mode", "Consolas", 18, blue, glm::vec2(120.0f, 80.0f)));
	for (Label* current : m_pInstructionLabels)
	{
		current->SetParent(this);
		AddChild(current);
	}
	ImGuiWindowFrame::Instance().SetGuiFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::GUI_Function()
{
	// Always open with a NewFrame

	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("GAME3001 - W2023 - Assignment2", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar );

	ImGui::Separator();

	// Debug Properties
	static bool toggleGrid = false;
	if(ImGui::Checkbox("Toggle Grid", &toggleGrid))
	{
		m_isGridEnabled = toggleGrid;
		m_setGridEnabled(m_isGridEnabled);
	}

	ImGui::Separator();

	// TItle Map Properties
	if (ImGui::CollapsingHeader("Tile Map"))
	{
		static char* tile_map[300];

		for (int i = 0; i < m_tileMap.length(); i++)
		{
			tile_map[i] = new char[i];
			sprintf(tile_map[i], "%c + %d", m_tileMap[i], i);

			if ((i % 20) != 0)
			{
				ImGui::SameLine();
			}

			if (m_tileMap[i] == 'S')
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.360f, 0.721f, 0.360f, 1.0f));
			}
			else if (m_tileMap[i] == 'G')
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.850f, 0.325f, 0.309f, 1.0f));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.007f, 0.458f, 0.847f, 1.0f));
			}

			if (ImGui::Button(tile_map[i], ImVec2(15, 15)))
			{
				if (m_tileMap[i] != 'S' && m_tileMap[i] != 'G')
				{
					m_tileMap[i] = (m_tileMap[i] == '-') ? 'I' : '-';
					m_removeAllObstacles();
					m_pBuildObstacles();
					m_buildTileMap();
					m_resetPathFinding();
				}
			}

			ImGui::PopStyleColor();
		}
	}

	ImGui::Separator();

	static int radio = static_cast<int>(m_currentHeuristic);
	ImGui::Text("Heuristic Type");
	ImGui::RadioButton("Manhattan", &radio, static_cast<int>(Heuristic::MANHATTAN));
	ImGui::SameLine();
	ImGui::RadioButton("Euclidean", &radio, static_cast<int>(Heuristic::EUCLIDEAN));

	// Check if current heuristic is not the same as selection
	if (static_cast<int>(m_currentHeuristic) != radio)
	{
		m_currentHeuristic = static_cast<Heuristic>(radio);
		if (!m_pPathList.empty())
		{
			m_resetPathFinding();
		}
		m_computeTileCosts();
	}

	ImGui::Separator();

	if (ImGui::Button("Find Shortest Path"))
	{
		if (!m_pPathList.empty())
		{
			m_resetPathFinding();
		}
		m_findShortestPath();
	}

	if (m_pathFound)
	{
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Path Found! - Length: %d", (m_pPathList.size() - 1));
	}

	ImGui::Separator();

	static int start_position[2] = { static_cast<int>(m_pStarShip->GetGridPosition().x),static_cast<int>(m_pStarShip->GetGridPosition().y) };
	static int goal_position[2] = { static_cast<int>(m_pTarget->GetGridPosition().x),static_cast<int>(m_pTarget->GetGridPosition().y) };


	if (ImGui::Button("Reset Path Finding"))
	{
		if (!m_pPathList.empty())
		{
			m_moveGameObject(m_pStarShip, start_position[0], start_position[1], TileStatus::START);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Reset Simulation"))
	{
		m_resetSimulation();
		start_position[0] = static_cast<int>(m_pStarShip->GetGridPosition().x);
		start_position[1] = static_cast<int>(m_pStarShip->GetGridPosition().y);
		goal_position[0] = static_cast<int>(m_pTarget->GetGridPosition().x);
		goal_position[1] = static_cast<int>(m_pTarget->GetGridPosition().y);
	}

	ImGui::Separator(); 

	// Starship properties

	if (ImGui::SliderInt2("Start Position", start_position, 0, Config::COL_NUM - 1))
	{
		// Constrain the object within max rows
		if (start_position[1] > Config::ROW_NUM - 1)
		{
			start_position[1] = Config::ROW_NUM - 1;
		}
		m_getTile(m_pStarShip->GetGridPosition())->SetIsStart(false);
		m_moveGameObject(m_pStarShip, start_position[0], start_position[1], TileStatus::START);
		m_getTile(m_pStarShip->GetGridPosition())->SetIsStart(true);
		

		
	}
	ImGui::Separator();

	// Target Properties
	if (ImGui::SliderInt2("Goal Position", goal_position, 0, Config::COL_NUM - 1))
	{
		// Constrain the object within max rows
		if (goal_position[1] > Config::ROW_NUM - 1)
		{
			goal_position[1] = Config::ROW_NUM - 1;
		}
		m_getTile(m_pTarget->GetGridPosition())->SetIsEnd(false);
		m_moveGameObject(m_pTarget, goal_position[0], goal_position[1], TileStatus::GOAL);
		m_getTile(m_pTarget->GetGridPosition())->SetIsEnd(true);

		m_computeTileCosts();
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
				tile->SetNeighbourTile(NeighbourTile::TOP_TILE, nullptr);
			} else
			{
				tile->SetNeighbourTile(NeighbourTile::TOP_TILE, m_getTile(col, row -1));
			}

			// RightMost Neighbour
			if (col == Config::COL_NUM - 1)
			{
				tile->SetNeighbourTile(NeighbourTile::RIGHT_TILE, nullptr);

			} else
			{
				tile->SetNeighbourTile(NeighbourTile::RIGHT_TILE, m_getTile(col + 1, row));

			}

			// BottomMost Neighbour
			if (row == Config::ROW_NUM - 1)
			{
				tile->SetNeighbourTile(NeighbourTile::BOTTOM_TILE, nullptr);

			}
			else
			{
				tile->SetNeighbourTile(NeighbourTile::BOTTOM_TILE, m_getTile(col, row + 1));

			}

			// LeftMost Neighbour
			if (col == 0)
			{
				tile->SetNeighbourTile(NeighbourTile::LEFT_TILE, nullptr);

			}
			else
			{
				tile->SetNeighbourTile(NeighbourTile::LEFT_TILE, m_getTile(col - 1, row));

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
	float distance = 0.0f;
	float dx = 0.0f;
	float dy = 0.0f;

	// for reach tile in the grid, loop
	for (auto tile : m_pGrid)
	{
		// compute the distance from each tile to the goal tile
		// distance (f) =tile cost (g) + heuristic estimate (h)
		switch(m_currentHeuristic)
		{
		case Heuristic::EUCLIDEAN:
			// Calculates the euclidean distance ("as the crow flies") for each tile.
			distance = Util::Distance(tile->GetGridPosition(), m_pTarget->GetGridPosition());

			break;
		case Heuristic::MANHATTAN:
			dx = abs(tile->GetGridPosition().x - m_pTarget->GetGridPosition().x);
			dy = abs(tile->GetGridPosition().y - m_pTarget->GetGridPosition().y);
			distance = dx + dy;
			break;
		}

		tile->SetTileCost(distance);
	}
}

bool PlayScene::m_getDebugEnabled() const
{
	return m_bDebugView;
}

void PlayScene::m_SetDebugEnabled(bool state)
{
	m_bDebugView = state;
	if(m_bDebugView)
	{
		m_setGridEnabled(true);
	}
	else
	{
		m_setGridEnabled(false);
	}
}

void PlayScene::m_pBuildObstacles()
{
	for (int i = 0; i < 300; ++i)
	{
		m_pObstacles.push_back(new Obstacle());
	}
}

void PlayScene::m_removeObstacleAt(int col, int row)
{
	for (Obstacle* obstacle : m_pObstacles)
	{
		if (obstacle != nullptr)
		{
			if (static_cast<int>(obstacle->GetGridPosition().x) == col && static_cast<int>(obstacle->GetGridPosition().y) == row)
			{
				RemoveChild(obstacle);
				m_pObstacles[(row * Config::COL_NUM) + col] = new Obstacle();
			}
		}
	}
}

void PlayScene::m_removeObstacleAt(glm::vec2 grid_position)
{
	m_removeObstacleAt(static_cast<int>(grid_position.x), static_cast<int>(grid_position.y));
}

void PlayScene::m_removeAllObstacles()
{
	for (Obstacle* obstacle : m_pObstacles)
	{
		RemoveChild(obstacle);
	}
	m_pObstacles.clear();
}

void PlayScene::m_findShortestPath()
{
	m_getTile(m_pStarShip->GetGridPosition())->SetTileParent(nullptr);
	m_pOpenList.push(m_getTile(m_pStarShip->GetGridPosition()));
	bool goal_found = false;

	while (!m_pOpenList.empty() && !goal_found)
	{
		const auto current_node = m_pOpenList.top(); // Tile with least F
		std::cout << current_node->GetGridPosition().x << ", " << current_node->GetGridPosition().y << std::endl;

		m_pOpenList.pop();
		current_node->SetTileStatus(TileStatus::CLOSED);

		for (auto neighbour : current_node->GetNeighbourTiles())
		{
			// Ignore these
			if (neighbour == nullptr || neighbour->GetTileStatus() == TileStatus::IMPASSABLE)
			{
				continue;
			}

			if (neighbour->GetTileStatus() == TileStatus::GOAL)
			{
				goal_found = true;
				neighbour->SetTileParent(current_node);
				std::cout << "Goal Found at: " << neighbour->GetGridPosition().x << ". " << neighbour->GetGridPosition().y << std::endl;
				m_pPathList.push_front(neighbour);
			}

			if (neighbour->GetTileStatus() == TileStatus::UNVISITED)
			{
				neighbour->SetTileParent((current_node));
				neighbour->SetTileStatus(TileStatus::OPEN);
				m_pOpenList.push(neighbour);
			}
		}
	}

	if(goal_found)
	{
		m_pathFound = true;
		SoundManager::Instance().Play_Sound("yay", 0);
		m_buildPathList();
		m_displayPathList();
	} else
	{
		const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, 4);
		std::cout << "No path Found!" << std::endl;
		SetConsoleTextAttribute(handle, 15);
	}
}

void PlayScene::m_buildPathList()
{
	while(m_pPathList.front()->GetTileParent() != nullptr)
	{
		m_pPathList.push_front(m_pPathList.front()->GetTileParent());
	}
}

void PlayScene::m_displayPathList() const
{
	std::cout << "Total Path Length is: " << m_pPathList.size() - 1 << std::endl;
	std::cout << "Path List is: " << std::endl;
	std::cout << "========================================" << std::endl;
	for (const auto node : m_pPathList)
	{
		node->SetTileStatus(TileStatus::PATH);
		std::cout << node->GetGridPosition().x << ", " << node->GetGridPosition().y << std::endl;
	}
}

void PlayScene::m_resetPathFinding()
{
	// Remove all nodes from the open list.
	while(!m_pOpenList.empty())
	{
		m_pOpenList.pop();
	}

	// Remove all nodes from the path list
	while (!m_pPathList.empty())
	{
		m_pPathList.pop_front();
	}

	// Reset path found
	m_pathFound = false;

	// Mark all the tiles in the grid marked anything other than UNVISITED
	for (Tile* tile : m_pGrid)
	{
		if (tile->GetTileStatus() == TileStatus::OPEN ||
			tile->GetTileStatus() == TileStatus::CLOSED ||
			tile->GetTileStatus() == TileStatus::PATH ||
			tile->GetTileStatus() == TileStatus::START || 
			tile->GetTileStatus() == TileStatus::GOAL)
		{
			tile->SetTileStatus(TileStatus::UNVISITED);
		}
	}

	m_getTile(m_pStarShip->GetGridPosition())->SetTileStatus(TileStatus::START);
	m_getTile(m_pStarShip->GetGridPosition())->SetIsStart(true);
	m_getTile(m_pTarget->GetGridPosition())->SetTileStatus(TileStatus::GOAL);
	m_getTile(m_pTarget->GetGridPosition())->SetIsEnd(true);

}

void PlayScene::m_resetSimulation()
{
	m_removeAllObstacles();
	m_pBuildObstacles();
	RemoveChild(m_pStarShip);
	RemoveChild(m_pTarget);
	m_initalizeTileMap();
	m_buildTileMap();
	m_computeTileCosts();
	m_resetPathFinding();
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

// Setts a specific amount of tiles as an obstacle.
void PlayScene::m_SetAsObstacle(int columnStart, int rowStart, int columnEnd, int rowEnd) const
{
	for (int i = columnStart; i <= columnEnd; i++)
	{
		for (int j = rowStart; j <= rowEnd; j++)
		{
			m_getTile(i, j)->SetTileStatus(TileStatus::IMPASSABLE);
		}
	}
}

void PlayScene::m_moveShipAcrossTilePath()
{
	if (!m_pPathList.empty())
	{
		static float time = 0.0f;
		Tile* current_node = m_pPathList.front();
		glm::vec2 node_position = m_getTile(current_node->GetGridPosition().x, current_node->GetGridPosition().y)->GetTransform()->position + Config::TILE_OFFSET;
		glm::vec2 position = Util::Lerp(m_pStarShip->GetTransform()->position, node_position, time);
		glm::vec2 rotation = Util::Normalize(node_position - m_pStarShip->GetTransform()->position);

		if (m_pStarShip->GetTransform()->rotation != rotation)
		{
			m_pStarShip->LookWhereYoureGoing(rotation);
		}

		if (time >= 1.0f)
		{
			time = 0.0f;
			m_pPathList.pop_front();
		}

		time += Game::Instance().GetDeltaTime();
		m_pStarShip->GetTransform()->position = position;

	}
}

void PlayScene::m_initalizeTileMap()
{
	m_tileMap = "----------I----------";
	m_tileMap += "---------I----------";
	m_tileMap += "---------I----------";
	m_tileMap += "--S------I----------";
	m_tileMap += "---------I----------";
	m_tileMap += "---------I----------";
	m_tileMap += "-----I---I---I------";
	m_tileMap += "-----I-------I------";
	m_tileMap += "-----I-------I------";
	m_tileMap += "-----I-------I------";
	m_tileMap += "-----I-------I------";
	m_tileMap += "-----I-------I-G----";
	m_tileMap += "-----I-------I------";
	m_tileMap += "-----I-------I------";
	m_tileMap += "-----I-------I------";
}


void PlayScene::m_buildTileMap()
{
	int obstacles_count = 0;

	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			if (m_tileMap[(row * Config::COL_NUM) + col] == 'I')
			{
				m_addObjectToGrid(m_pObstacles[(row * Config::COL_NUM) + col], col, row, TileStatus::IMPASSABLE);
				obstacles_count++;
			}

			if ((m_tileMap[(row * Config::COL_NUM) + col] == 'S'))
			{
				m_addObjectToGrid(m_pStarShip, col, row, TileStatus::START);
				m_pStarShip->SetTargetPosition(m_getTile(col, row)->GetTransform()->position + Config::TILE_OFFSET);
				m_getTile(col, row)->SetIsStart(true);
			}

			if ((m_tileMap[(row * Config::COL_NUM) + col] == 'G'))
			{
				m_addObjectToGrid(m_pTarget, col, row, TileStatus::GOAL);
				m_getTile(col, row)->SetIsEnd(true);
			}

			if ((m_tileMap[(row * Config::COL_NUM) + col] == '-'))
			{
				m_getTile(col, row)->SetTileStatus(TileStatus::UNVISITED);
			}
		}
	}
}

void PlayScene::m_updateTileMap(const int col, const int row, const TileStatus status)
{
	switch(status)
	{
	case TileStatus::UNVISITED:
		m_tileMap[(row * Config::COL_NUM) + col] = '-';
		break;
	case TileStatus::OPEN:
		m_tileMap[(row * Config::COL_NUM) + col] = 'O';
		break;
	case TileStatus::CLOSED:
		m_tileMap[(row * Config::COL_NUM) + col] = 'C';
		break;
	case TileStatus::IMPASSABLE:
		m_tileMap[(row * Config::COL_NUM) + col] = 'I';
		break;
	case TileStatus::GOAL:
		m_tileMap[(row * Config::COL_NUM) + col] = 'G';
		break;
	case TileStatus::START:
		m_tileMap[(row * Config::COL_NUM) + col] = 'S';
		break;
	case TileStatus::PATH:
		m_tileMap[(row * Config::COL_NUM) + col] = 'P';
		break;
	}
}

void PlayScene::m_updateTileMap(glm::vec2 grid_position, TileStatus status)
{
	m_updateTileMap(static_cast<int>(grid_position.x), static_cast<int>(grid_position.y), status);
}


template <typename T>
void PlayScene::m_addObjectToGrid(T*& object, int col, int row, TileStatus status)
{
	object = new T();
	object->GetTransform()->position = m_getTile(col, row)->GetTransform()->position + Config::TILE_OFFSET;
	object->SetGridPosition(static_cast<float>(col), static_cast<float>(row));
	m_getTile(col, row)->SetTileStatus(status);
	AddChild(object);
}

template <typename T>
void PlayScene::m_moveGameObject(T*& object, int col, int row, TileStatus status)
{
	// Ignore changes to the Impassable tiles
	if (m_getTile(object->GetGridPosition())->GetTileStatus() != TileStatus::IMPASSABLE)
	{
		m_getTile(object->GetGridPosition())->SetTileStatus(TileStatus::UNVISITED);
		m_updateTileMap(object->GetGridPosition(), TileStatus::UNVISITED);
	}
	// Convert grid space to world space
	object->GetTransform()->position = m_getTile(col, row)->GetTransform()->position + Config::TILE_OFFSET;
	object->SetGridPosition(static_cast<float>(col), static_cast<float>(row));

	// Ignore changes to the impassable tiles
	if (m_getTile(object->GetGridPosition())->GetTileStatus() != TileStatus::IMPASSABLE)
	{
		m_getTile(object->GetGridPosition())->SetTileStatus(status);
	}
	m_updateTileMap(col, row, status);

	if (!m_pPathList.empty())
	{
		m_resetPathFinding();
	}
}
