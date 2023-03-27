#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include <windows.h>

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"
#include <fstream>

PlayScene::PlayScene()
{
	PlayScene::Start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::Draw()
{
	DrawDisplayList();

	if(m_isGridEnabled)
	{
		for (const auto obstacle : m_pObstacles)
		{
			Util::DrawRect(obstacle->GetTransform()->position - glm::vec2(obstacle->GetWidth() * 0.5f,
				obstacle->GetHeight() * 0.5f), obstacle->GetWidth(), obstacle->GetHeight());
		}
	}

	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	CheckCollision();
	CheckEnemyDetectionRadius();
	UpdateDisplayList();
	for (auto enemy : m_pEnemyPool->GetPool()) {
		m_checkAgentLOS(enemy, m_pPlayer);
		switch (m_LOSMode)
		{
		case LOSMode::PLAYER:
			m_checkAllNodesWithTarget(m_pPlayer);
			break;
		case LOSMode::ENEMY:
			m_checkAllNodesWithTarget(enemy);
			break;
		case LOSMode::BOTH:
			m_checkAllNodesWithBoth();
			break;
		}
	}

	// Make a decision
	for (std::pair<Enemy*, DecisionTree*> mapPair : m_decisionTrees) {
		mapPair.second->MakeDecision();
	}
}

void PlayScene::Clean()
{
	RemoveAllChildren();
}

void PlayScene::HandleEvents()
{
	EventManager::Instance().Update();

	// Toggles into Debug View
	if (EventManager::Instance().KeyPressed(SDL_SCANCODE_H)) {
		Game::Instance().SetDebugMode(!m_isGridEnabled);
		m_isGridEnabled = !m_isGridEnabled;
		m_toggleGrid(m_isGridEnabled);
	}
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
	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_W))
	{
		m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_DOWN);

		m_pPlayer->GetRigidBody()->velocity.y-=12.5f;
		SoundManager::Instance().Play_Sound("footsteps");
	}
	else if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_S))
	{
		m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_UP);
		m_pPlayer->GetRigidBody()->velocity.y+=12.5f;
		SoundManager::Instance().Play_Sound("footsteps");
	}
	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_A))
	{
		m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_LEFT);
		m_pPlayer->GetRigidBody()->velocity.x-=12.5f;
		SoundManager::Instance().Play_Sound("footsteps");
	}
	else if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_D))
	{
		m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_RIGHT);
		m_pPlayer->GetRigidBody()->velocity.x+=12.5f;
		SoundManager::Instance().Play_Sound("footsteps");
	}
	if (EventManager::Instance().MousePressed(1))
	{
		std::cout<< "Mouse 1 Pressed" << std::endl;
		// Will make an enemy pool for the current enemies on screen so iterating through this is not a problem
		// If player attack radius is touching the enemy in any way, melee attack!
		SoundManager::Instance().Play_Sound("swipe");
		for (auto enemy : m_pEnemyPool->GetPool()) {
			if (Util::GetClosestEdge(m_pPlayer->GetTransform()->position, enemy) <= m_pPlayer->GetRangeOfAttack()) {
				m_pPlayer->MeleeAttack();
				enemy->TakeDamage(m_pPlayer->GetDamage());
			}
		}
		
	}
	if (EventManager::Instance().MousePressed(3))
	{
		std::cout<< "Mouse 2 Pressed" << std::endl;
		m_pTorpedoPool->Fire();
	}


	if(Game::Instance().GetDebugMode())
	{
		if (EventManager::Instance().KeyPressed(SDL_SCANCODE_K)) {
			//BASE ENEMY WILL TAKE DAMAGE
		}
		if (EventManager::Instance().KeyPressed(SDL_SCANCODE_P)) {
			//TOGGLE ENEMY BETWEEN PATROL AND IDLE

			for (auto enemy : m_pEnemyPool->GetPool()) {
				if (enemy->GetActionState() != ActionState::PATROL)
				{
					enemy->SetActionState(ActionState::PATROL);
				}
				else
				{
					enemy->SetActionState(ActionState::IDLE);
				}
			}
			
		}
		if (EventManager::Instance().KeyPressed(SDL_SCANCODE_K))
		{
			for (auto enemy : m_pEnemyPool->GetPool())
			{
				enemy->TakeDamage(30.0f);
			}
		}

		if (EventManager::Instance().KeyPressed(SDL_SCANCODE_R))
		{
			Enemy* new_enemy = new StarShip;
			m_pEnemyPool->Spawn(new_enemy);

			m_decisionTrees.emplace(std::pair{ new_enemy, new DecisionTree(new_enemy) }); // Using our overloaded constructor
			m_decisionTrees[new_enemy]->Display(); // Optional
			m_decisionTrees[new_enemy]->MakeDecision(); // Patrol
		}
	}
	
}

void PlayScene::Start()
{
	// Set GUI Title
	m_guiTitle = "Assignment-3";

	// Setup a few more fields
	m_LOSMode = LOSMode::PLAYER;
	m_pathNodeLOSDistance = 1000; // 1000px distance
	m_setPathNodeLOSDistance(m_pathNodeLOSDistance);

	// Add Game Objects
	m_pBackground = new Background();
	AddChild(m_pBackground, 0);
	

	m_pEnemyPool = new EnemyPool();
	AddChild(m_pEnemyPool);
	
	/*m_pStarShip = new StarShip();
	
	AddChild(m_pStarShip, 2);*/

	m_pPlayer = new Player();
	m_pPlayer->GetTransform()->position = glm::vec2(100.0f,300.0f);
	AddChild(m_pPlayer,2);
	Game::Instance().SetPlayer(m_pPlayer);

	m_pTorpedoPool = new TorpedoPool();
	AddChild(m_pTorpedoPool);

	// Add Obstacles
	BuildObstaclePool();

	// Setup the Grid
	m_isGridEnabled = false;
	m_buildGrid();
	m_toggleGrid(m_isGridEnabled);

	m_pEnemyPool->Spawn(new StarShip);

	// Create Decision Tree

	for (Enemy* enemy : m_pEnemyPool->GetPool()) {
		m_decisionTrees.emplace(std::pair{enemy, new DecisionTree(enemy)}); // Using our overloaded constructor
		m_decisionTrees[enemy]->Display(); // Optional
		m_decisionTrees[enemy]->MakeDecision(); // Patrol
	}
	//m_decisionTree = new DecisionTree(m_pStarShip); // Using our overloaded constructor
	//m_decisionTree->Display(); // Optional
	//m_decisionTree->MakeDecision(); // Patrol

	// Preload Sounds

	SoundManager::Instance().Load("../Assets/Audio/yay.ogg", "yay", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/thunder.ogg", "thunder", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/audio/swipe.mp3", "swipe", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/audio/footsteps.mp3", "footsteps", SoundType::SOUND_SFX);

	// Preload music
	SoundManager::Instance().Load("../Assets/Audio/bgMusic.mp3", "bgMusic", SoundType::SOUND_MUSIC);
	SoundManager::Instance().SetMusicVolume(16);

	// Play Music
	SoundManager::Instance().PlayMusic("bgMusic");

	ImGuiWindowFrame::Instance().SetGuiFunction(std::bind(&PlayScene::GUI_Function, this));
}


void PlayScene::GUI_Function()
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("GAME3001 - W2023 - Assignment 3", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar );

	ImGui::Separator();

	// Debug Properties
	if(ImGui::Checkbox("Toggle Grid", &m_isGridEnabled))
	{
		Game::Instance().SetDebugMode(m_isGridEnabled);
		m_toggleGrid(m_isGridEnabled);
	}

	ImGui::Separator();

	static int LOS_mode = static_cast<int>(m_LOSMode);
	ImGui::Text("Path Node LOS");
	ImGui::RadioButton("Player", &LOS_mode, static_cast<int>(LOSMode::PLAYER)); ImGui::SameLine();
	ImGui::RadioButton("Enemy", &LOS_mode, static_cast<int>(LOSMode::ENEMY)); ImGui::SameLine();
	ImGui::RadioButton("Both Player & Enemy", &LOS_mode, static_cast<int>(LOSMode::BOTH));

	m_LOSMode = static_cast<LOSMode>(LOS_mode);

	ImGui::Separator();

	if(ImGui::SliderInt("Path Node LOS Distance", &m_pathNodeLOSDistance, 0, 1000))
	{
		m_setPathNodeLOSDistance(m_pathNodeLOSDistance);
	}

	ImGui::Separator();

	// StarShip Properties
	//static int shipPosition[] = { static_cast<int>(m_pStarShip->GetTransform()->position.x),
	//	static_cast<int>(m_pStarShip->GetTransform()->position.y) };
	//if(ImGui::SliderInt2("Ship Position", shipPosition, 0, 800))
	//{
	//	m_pStarShip->GetTransform()->position.x = static_cast<float>(shipPosition[0]);
	//	m_pStarShip->GetTransform()->position.y = static_cast<float>(shipPosition[1]);
	//}

	//// allow the ship to rotate
	//static int angle;
	//if(ImGui::SliderInt("Ship Direction", &angle, -360, 360))
	//{
	//	m_pStarShip->SetCurrentHeading(static_cast<float>(angle));
	//}

	ImGui::Separator();

	// Target Properties
	//static int targetPosition[] = { static_cast<int>(m_pTarget->GetTransform()->position.x),
	//	static_cast<int>(m_pTarget->GetTransform()->position.y) };
	//if (ImGui::SliderInt2("Target Position", targetPosition, 0, 800))
	//{
	//	m_pTarget->GetTransform()->position.x = static_cast<float>(targetPosition[0]);
	//	m_pTarget->GetTransform()->position.y = static_cast<float>(targetPosition[1]);
	//}

	ImGui::Separator();

	// Add Obstacle position control for each obstacle
	for(unsigned i = 0; i < m_pObstacles.size(); ++i)
	{
		int obstaclePosition[] = { static_cast<int>(m_pObstacles[i]->GetTransform()->position.x),
		static_cast<int>(m_pObstacles[i]->GetTransform()->position.y) };
		std::string label = "Obstacle " + std::to_string(i + 1) + " Position";
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

	while(!inFile.eof())
	{
		std::cout << "Obstacle " << std::endl;
		auto obstacle = new Obstacle();
		float x, y, w, h; // declare variables the same as how the file is organized
		inFile >> x >> y >> w >> h; // read data from file line by line
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
			auto path_node = new PathNode();
			path_node->GetTransform()->position = glm::vec2(static_cast<float>(col) * tile_size + offset.x,
				static_cast<float>(row) * tile_size + offset.y);

			// only show grid where there are no obstacles
			bool keep_node = true;
			for (const auto obstacle : m_pObstacles)
			{
				// add the Obstacle Buffer TODO: this can be improved
				const auto buffer = new Obstacle();
				buffer->GetTransform()->position = obstacle->GetTransform()->position;
				buffer->SetWidth(obstacle->GetWidth() + 40);
				buffer->SetHeight(obstacle->GetHeight() + 40);

				// determine which path_nodes to keep
				if(CollisionManager::AABBCheck(path_node, buffer))
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

bool PlayScene::m_checkAgentLOS(Agent* agent, DisplayObject* target_object) const
{
	bool has_LOS = false; // default - No LOS
	agent->SetHasLOS(has_LOS);

	// if ship to target distance is less than or equal to the LOS Distance (Range)
	const auto agent_to_range = Util::GetClosestEdge(agent->GetTransform()->position, target_object);
	if (agent_to_range <= agent->GetLOSDistance())
	{
		// we are in within LOS Distance 
		std::vector<DisplayObject*> contact_list;
		for (auto display_object : GetDisplayList())
		{
			if((display_object->GetType() != GameObjectType::AGENT)
				&& (display_object->GetType() != GameObjectType::PATH_NODE)
				&& (display_object->GetType() != GameObjectType::TARGET)
				&& (display_object->GetType() != GameObjectType::PLAYER))
			{
				const auto agent_to_object_distance = Util::GetClosestEdge(agent->GetTransform()->position, display_object);
				if (agent_to_object_distance <= agent_to_range) { contact_list.push_back(display_object);  }
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
	// check angle to the target so we can still use LOS Distance for path_nodes
	const auto target_direction = target_object->GetTransform()->position - path_node->GetTransform()->position;
	const auto normalized_direction = Util::Normalize(target_direction); // changes direction vector to a unit vector (magnitude of 1)
	path_node->SetCurrentDirection(normalized_direction);
	return m_checkAgentLOS(path_node, target_object);
}

void PlayScene::m_checkAllNodesWithTarget(DisplayObject* target_object) const
{
	for (const auto path_node : m_pGrid)
	{
		m_checkPathNodeLOS(path_node, target_object);
	}
}

void PlayScene::m_checkAllNodesWithBoth() const
{
	for (const auto path_node : m_pGrid)
	{
		for (const auto enemy : m_pEnemyPool->GetPool()) {
			const bool LOSWithEnemy = m_checkPathNodeLOS(path_node, enemy);
			const bool LOSWithPlayer = m_checkPathNodeLOS(path_node, m_pPlayer);
			path_node->SetHasLOS(LOSWithEnemy && LOSWithPlayer, glm::vec4(0, 1, 1, 1));
		}
	}
}

void PlayScene::m_setPathNodeLOSDistance(const int distance) const
{
	for (const auto path_node : m_pGrid)
	{
		path_node->SetLOSDistance(static_cast<float>(distance));
	}
}

// Checks to see if there is collision between anything that it is needed for
void PlayScene::CheckCollision()
{
	bool canTorpedoHitEnemy = false;

	for (auto enemy : m_pEnemyPool->GetPool()) {
		for (auto projectile : m_pTorpedoPool->GetPool()) {

			if (projectile->GetRigidBody()->isColliding) {
				canTorpedoHitEnemy = false;
			}
			else { canTorpedoHitEnemy = true; }

			if (CollisionManager::AABBCheck(enemy, projectile)) {
				if (canTorpedoHitEnemy) {
					enemy->TakeDamage(projectile->GetDamage());
				}
			}
		}
	}
	m_pPlayer->GetRigidBody()->isColliding=false;
	for (auto obstacle : m_pObstacles)
	{
		if(CollisionManager::AABBCheck(m_pPlayer,obstacle))
		{
			m_pPlayer->GetRigidBody()->isColliding = true;
		}
		for (auto proj : m_pTorpedoPool->GetPool())
		{
			if(CollisionManager::AABBCheck(proj,obstacle))
			{
				proj->GetRigidBody()->isColliding=true;
			}
		}
	}
}

void PlayScene::CheckEnemyDetectionRadius()
{
	for (auto enemy : m_pEnemyPool->GetPool())
	{
		if (Util::Distance(enemy->GetTransform()->position, m_pPlayer->GetTransform()->position) <= enemy->GetDetectionRadius()) {
			enemy->SetDetectingPlayer(true);
			std::cout << "true\n";\
		} else
		{
			enemy->SetDetectingPlayer(false);
		}
	}
}
