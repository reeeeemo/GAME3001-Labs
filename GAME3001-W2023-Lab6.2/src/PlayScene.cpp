#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include <windows.h>

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

PlayScene::PlayScene()
{
	PlayScene::Start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::Draw()
{
	DrawDisplayList();

	if (m_isGridEnabled) {

		for (const auto obstacle : m_pObstacles)
		{
			Util::DrawRect(obstacle->GetTransform()->position - glm::vec2(obstacle->GetWidth() * 0.5f,
				obstacle->GetHeight() * 0.5f), obstacle->GetWidth(), obstacle->GetHeight());
		}

		if (m_pathFound) {
			/*for (int i = 0; i < m_pPath.size(); i++) {
				PathNode* next_node = nullptr;
				if (m_pPath[i] != m_pPath.back()) {
					next_node = m_pPath[i + 1];
				}
				PathNode* cur_node = m_pPath[i];
				if (next_node != nullptr) {
					Util::DrawLine(cur_node->GetTransform()->position, next_node->GetTransform()->position, glm::vec4(1, 1, 1, 1));
				}
			}*/

			Util::DrawLine(m_pStarShip->GetTransform()->position, m_pTarget->GetTransform()->position, glm::vec4(0.3f, 0.1f, 0, 1));
		}
	}

	
	

	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	UpdateDisplayList();
	m_checkAgentLOS(m_pStarShip, m_pTarget);


	switch (m_LOSMode) {
	case LOSMode::TARGET:
		m_checkAllNodesWithTarget(m_pTarget);
		break;
	case LOSMode::SHIP:
		m_checkAllNodesWithTarget(m_pStarShip);
		break;
	case LOSMode::BOTH:
		m_checkAllNodesWithBoth();
		break;
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
}

void PlayScene::Start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";

	// Setup a few fields
	m_LOSMode = LOSMode::TARGET;
	m_pathNodeLOSDistance = 1000; // 1000px distance
	m_setPathNodeLOSDistance(m_pathNodeLOSDistance);


	// Add Game Objects
	m_pTarget = new Target();
	m_pTarget->GetTransform()->position = glm::vec2(550.0f, 300.0f);
	AddChild(m_pTarget, 2);

	m_pStarShip = new StarShip();
	m_pStarShip->GetTransform()->position = glm::vec2(150.0f, 300.0f);
	AddChild(m_pStarShip, 2);

	// Add Obstacles
	BuildObstaclePool();

	// Setup the Grid
	m_isGridEnabled = false;
	m_buildGrid();
	m_toggleGrid(m_isGridEnabled);

	// Grab initial positions of starship and target for nodes
	m_updateCurrentNode(m_pStarShip);
	m_updateCurrentNode(m_pTarget);

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
	
	ImGui::Begin("GAME3001 - W2023 - Lab 6.2", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar );

	ImGui::Separator();

	// Debug Properties
	if(ImGui::Checkbox("Toggle Grid", &m_isGridEnabled))
	{
		m_toggleGrid(m_isGridEnabled);
	}

	ImGui::Separator();

	static int LOS_mode = static_cast<int>(m_LOSMode);
	ImGui::Text("Path Node LOS");

	ImGui::RadioButton("Target", &LOS_mode, static_cast<int>(LOSMode::TARGET)); ImGui::SameLine();
	ImGui::RadioButton("StarShip", &LOS_mode, static_cast<int>(LOSMode::SHIP)); ImGui::SameLine();
	ImGui::RadioButton("Both", &LOS_mode, static_cast<int>(LOSMode::BOTH)); 

	m_LOSMode = static_cast<LOSMode>(LOS_mode);

	ImGui::Separator();

	if (ImGui::Button("Draw LOS Path")) {
		m_getPath();
	}

	if (m_pathFound) {
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Path Found!");
	}
	else {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Path Unable to Access!");
	}

	ImGui::Separator();


	if (ImGui::SliderInt("Path Node LOS Distance", &m_pathNodeLOSDistance, 0, 1000)) {
		m_setPathNodeLOSDistance(m_pathNodeLOSDistance);
	}

	ImGui::Separator();

	// StarShip Properties
	static int shipPosition[] = { static_cast<int>(m_pStarShip->GetTransform()->position.x),
		static_cast<int>(m_pStarShip->GetTransform()->position.y) };
	if(ImGui::SliderInt2("Ship Position", shipPosition, 0, 800))
	{
		m_pStarShip->GetTransform()->position.x = static_cast<float>(shipPosition[0]);
		m_pStarShip->GetTransform()->position.y = static_cast<float>(shipPosition[1]);
		m_updateCurrentNode(m_pStarShip);
	}

	// allow the ship to rotate
	static int angle;
	if(ImGui::SliderInt("Ship Direction", &angle, -360, 360))
	{
		m_pStarShip->SetCurrentHeading(static_cast<float>(angle));
	}

	ImGui::Separator();

	// Target Properties
	static int targetPosition[] = { static_cast<int>(m_pTarget->GetTransform()->position.x),
		static_cast<int>(m_pTarget->GetTransform()->position.y) };
	if (ImGui::SliderInt2("Target Position", targetPosition, 0, 800))
	{
		m_pTarget->GetTransform()->position.x = static_cast<float>(targetPosition[0]);
		m_pTarget->GetTransform()->position.y = static_cast<float>(targetPosition[1]);
		m_updateCurrentNode(m_pTarget);
	}

	ImGui::Separator();

	// Add Obstacle position control for each obstacle
	for(unsigned i = 0; i < m_pObstacles.size(); ++i)
	{
		int obstaclePosition[] = { static_cast<int>(m_pObstacles[i]->GetTransform()->position.x),
		static_cast<int>(m_pObstacles[i]->GetTransform()->position.y) };
		std::string label = "Obstacle" + std::to_string(i + 1) + " Position";
		if(ImGui::SliderInt2(label.c_str(), obstaclePosition, 0, 800))
		{
			m_pObstacles[i]->GetTransform()->position.x = static_cast<float>(obstaclePosition[0]);
			m_pObstacles[i]->GetTransform()->position.y = static_cast<float>(obstaclePosition[1]);
			m_buildGrid();
		}
	}


	ImGui::End();
}

void PlayScene::BuildObstaclePool()
{
	std::ifstream inFile("../Assets/data/obstacles.txt");

	while (!inFile.eof()) {
		std::cout << "Obstacle " << std::endl;
		auto obstacle = new Obstacle();
		float x, y, w, h; // Same way that the file is organized
		inFile >> x >> y >> w >> h; // Read data from the file line by line
		obstacle->GetTransform()->position = glm::vec2(x, y);
		obstacle->SetWidth(static_cast<int>(w));
		obstacle->SetHeight(static_cast<int>(h));
		AddChild(obstacle, 0);
		m_pObstacles.push_back(obstacle);
	}
	inFile.close();
}

void PlayScene::m_buildGrid()
{
	constexpr auto tile_size = Config::TILE_SIZE;
	constexpr auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	m_clearNodes(); // we will need to clear nodes because we will rebuild/redraw the grid if we move an obstacle

	// lay out a grid of path_nodes
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			PathNode* path_node = new PathNode();
			path_node->GetTransform()->position = glm::vec2(static_cast<float>(col) * tile_size + offset.x,
				static_cast<float>(row) * tile_size + offset.y);

			// only show grid where there are no obstacles
			bool keep_node = true;
			for (const auto obstacle : m_pObstacles)
			{
				// determine which path_nodes to keep
				if(CollisionManager::AABBCheck(path_node, obstacle))
				{
					keep_node = false;
				}
			}
			if(keep_node)
			{
				AddChild(path_node);
				m_pGrid.push_back(path_node);
			}
			else
			{
				delete path_node;
			}
		}

		// if Grid is supposed to be hidden - make it so!
		m_toggleGrid(m_isGridEnabled);
	}
}

void PlayScene::m_toggleGrid(const bool state) const
{
	for (const auto path_node : m_pGrid)
	{
		path_node->SetVisible(state);
	}
}

void PlayScene::m_clearNodes()
{
	m_pGrid.clear();
	for (const auto display_object : GetDisplayList())
	{
		if(display_object->GetType() == GameObjectType::PATH_NODE)
		{
			RemoveChild(display_object);
		}
	}
}

void PlayScene::m_updateCurrentNode(DisplayObject* target_object)
{
	float totalDistance = 1000.0f; // High number so can be overridden.
	PathNode* cur_node = nullptr;
	for (PathNode* node : m_pGrid) {
		node->m_isTarget = false;
		node->m_isPlayer = false;

		float tempDistance = Util::Distance(node->GetTransform()->position, target_object->GetTransform()->position);

		if (tempDistance < totalDistance) {
			totalDistance = tempDistance;
			cur_node = node;
		}
	}

	if (cur_node != nullptr) {
		if (target_object->GetType() == GameObjectType::TARGET) { // if target
			cur_node->m_isTarget = true;
		}
		else if (target_object->GetType() == GameObjectType::AGENT) { // if player
			cur_node->m_isPlayer = true;
		}
	}

}

bool PlayScene::m_checkAgentLOS(Agent* agent, DisplayObject* target_object) const
{
	bool has_LOS = false; // Default - no LOS

	agent->SetHasLOS(has_LOS);

	// if ship to target distance is less than or equal to the LOS Distance (Range)
	const auto agent_to_range = Util::GetClosestEdge(m_pStarShip->GetTransform()->position, target_object);
	if (agent_to_range <= agent->GetLOSDistance())
	{
		// we are in range
		std::vector<DisplayObject*> contact_list;
		for (auto display_object : GetDisplayList())
		{
			const auto agent_to_object_distance = Util::GetClosestEdge(agent->GetTransform()->position, display_object);

			if (agent_to_object_distance > agent_to_range) { continue; } // Target is out of range
			if ((display_object->GetType() != GameObjectType::AGENT)
				&& (display_object->GetType() != GameObjectType::PATH_NODE)
				&& (display_object->GetType() != GameObjectType::TARGET)) 
			{
				contact_list.push_back(display_object);
			}

		}

		const glm::vec2 agent_LOS_endPoint = agent->GetTransform()->position + agent->GetCurrentDirection() * agent->GetLOSDistance();
		has_LOS = CollisionManager::LOSCheck(agent, agent_LOS_endPoint, contact_list, target_object);

		const auto LOSColour = (target_object->GetType() == GameObjectType::AGENT) ? glm::vec4(0, 0, 1, 1) : glm::vec4(0, 1, 0, 1);
		agent->SetHasLOS(has_LOS, LOSColour);
	}

	return has_LOS;
}

bool PlayScene::m_checkPathNodeLOS(PathNode* path_node, DisplayObject* target_object) const
{
	// Check angle to the target so we can still use LOS Distance for path_nodes
	const auto target_direction = target_object->GetTransform()->position - path_node->GetTransform()->position;
	const auto normalized_direction = Util::Normalize(target_direction); // Changes direction to a unit vector (length of 1).
	path_node->SetCurrentDirection(normalized_direction);
	return m_checkAgentLOS(path_node, target_object);
}

void PlayScene::m_checkAllNodesWithTarget(DisplayObject* target_object) const
{
	for (const auto path_node : m_pGrid) {
		m_checkPathNodeLOS(path_node, target_object);
	}
}

void PlayScene::m_checkAllNodesWithBoth() const 
{
	for (const auto path_node : m_pGrid) {
		const bool LOSWithStarShip = m_checkPathNodeLOS(path_node, m_pStarShip);
		const bool LOSWithTarget = m_checkPathNodeLOS(path_node, m_pTarget);
		path_node->SetHasLOS(LOSWithStarShip && LOSWithTarget, glm::vec4(0,1,1,1));

	}
}

void PlayScene::m_setPathNodeLOSDistance(const int distance) const 
{
	for (const auto path_node : m_pGrid) {
		path_node->SetLOSDistance(static_cast<float>(distance));
	}
}

void PlayScene::m_getPath() {
	//PathNode* plr_node = nullptr;
	//bool goal_found = false;
	//std::vector<PathNode*> m_pOpenList;
	//constexpr glm::vec2 offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);


	//// Grabbing player node from list
	//for (PathNode* node : m_pGrid) {
	//	if (node->m_isPlayer == true) {
	//		plr_node = node;
	//	}
	//}

	//m_pOpenList.push_back(plr_node);


	//while (!m_pOpenList.empty() && !goal_found) {
	//	const auto cur_node = m_pOpenList.back();

	//	m_pOpenList.pop_back();
	//	m_pPath.push_back(cur_node);

	//	glm::vec2 next_node_pos =
	//		glm::vec2(cur_node->GetTransform()->position +
	//			glm::vec2(cur_node->GetCurrentDirection().x * ,
	//				cur_node->GetCurrentDirection().y * Config::TILE_SIZE));


	//	for (PathNode* node : m_pGrid) {
	//		if (next_node_pos == node->GetTransform()->position) {
	//			if (node->GetType() == GameObjectType::TARGET) {
	//				m_pOpenList.clear();
	//				goal_found = true;
	//			}
	//			else {
	//				m_pOpenList.push_back(node);
	//			}
	//		}
	//	}

	//}
	m_pathFound = true;
}
