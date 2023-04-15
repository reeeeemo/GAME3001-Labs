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

#include "DestructibleObstacle.h"

PlayScene::PlayScene()
{
	PlayScene::Start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::Draw()
{
	for (const auto node : m_pGrid)
	{
		TextureManager::Instance().Draw("grass", node->GetTransform()->position,
			0, 255, true, SDL_FLIP_NONE);
	}


	DrawDisplayList();

	

	if(m_isGridEnabled)
	{
		for (const auto obstacle : m_pObstacles)
		{
			Util::DrawRect(obstacle->GetTransform()->position - glm::vec2(obstacle->GetWidth() * 0.5f,
				obstacle->GetHeight() * 0.5f), obstacle->GetWidth(), obstacle->GetHeight());
		}
		for (const auto enemy : m_pEnemyPool->GetPool())
		{
			bool detected;
			if (enemy->GetEnemyType() == EnemyType::CLOSE_COMBAT)
			{
				detected = dynamic_cast<CloseCombatEnemy*>(enemy)->GetTree()->GetPlayerDetectedNode()->GetDetected();
				Util::DrawCircle(enemy->GetTransform()->position, dynamic_cast<CloseCombatEnemy*>(enemy)->GetMaxRange(), detected ? glm::vec4(0, 1, 0, 1) : glm::vec4(1, 0, 0, 1));
			}
			else { // If ranged combat enemy
				detected = dynamic_cast<RangedCombatEnemy*>(enemy)->GetTree()->GetPlayerDetectedNode()->GetDetected();
				Util::DrawCircle(enemy->GetTransform()->position, dynamic_cast<RangedCombatEnemy*>(enemy)->GetMaxRange(), detected ? glm::vec4(0, 1, 0, 1) : glm::vec4(1, 0, 0, 1));
			}

		}
	}

	

	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	if (m_pPlayer->GetHealth() <= 0)
	{
		// Change to lose scene condition :)
	}
	CheckCollision();
	UpdateDisplayList();
	for (const auto enemy : m_pEnemyPool->GetPool())
	{
		// Distance check between starship and target for detection radius
		float distance = Util::Distance(enemy->GetTransform()->position, m_pPlayer->GetTransform()->position);

		if (enemy->GetEnemyType() == EnemyType::CLOSE_COMBAT)
		{
			const auto tempEnemy = dynamic_cast<CloseCombatEnemy*>(enemy);
			tempEnemy->GetTree()->GetEnemyHealthNode()->SetHealthy(tempEnemy->GetHealth() > 25);
			tempEnemy->GetTree()->GetEnemyHitNode()->SetHit(false);
			tempEnemy->CheckAgentLOSToTarget(m_pPlayer, m_pObstacles);

			tempEnemy->GetTree()->GetPlayerDetectedNode()->SetPlayerDetected(distance < tempEnemy->GetMaxRange());
			tempEnemy->GetTree()->GetCloseCombatNode()->SetIsWithinCombatRange(distance <= tempEnemy->GetMinRange());
		}
		else { // If ranged combat enemy
			const auto tempEnemy = dynamic_cast<RangedCombatEnemy*>(enemy);
			tempEnemy->GetTree()->GetEnemyHealthNode()->SetHealthy(tempEnemy->GetHealth() > 25);
			tempEnemy->GetTree()->GetEnemyHitNode()->SetHit(false);
			tempEnemy->CheckAgentLOSToTarget(m_pPlayer, m_pObstacles);

			// Radius detection.. Just outside of LOS Range (around 300px)
			tempEnemy->GetTree()->GetPlayerDetectedNode()->SetPlayerDetected(distance < tempEnemy->GetMaxRange());

			// Within LOS Distance.. but not too close (optimum firing range)
			tempEnemy->GetTree()->GetRangedCombatNode()->SetIsWithinCombatRange(distance >= tempEnemy->GetMinRange());

		}
	}


	switch(m_LOSMode)
	{
	case LOSMode::TARGET:
		m_checkAllNodesWithTarget(m_pPlayer);
		break;
	case LOSMode::SHIP:
		for (const auto enemy : m_pEnemyPool->GetPool())
		{
			m_checkAllNodesWithTarget(enemy);
		}
		break;
	case LOSMode::BOTH:
		m_checkAllNodesWithBoth();
		break;
	}

	m_RemainingEnemiesLabel->SetColour({128,128,128,255});
	m_RemainingEnemiesLabel->SetText(std::to_string(m_pEnemyPool->GetPool().size()));
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

	// Toggles into Debug View
	if (EventManager::Instance().KeyPressed(SDL_SCANCODE_H)) {
		Game::Instance().SetDebugMode(!m_isGridEnabled);
		m_isGridEnabled = !m_isGridEnabled;
		m_toggleGrid(m_isGridEnabled);
	}

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_1))
	{
		Game::Instance().ChangeSceneState(SceneState::START);
	}

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_2))
	{
		Game::Instance().ChangeSceneState(SceneState::END);
	}

	// Player movement stuff
	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_W))
	{
		m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_DOWN);

		m_pPlayer->GetRigidBody()->velocity.y -= 12.5f;
		//SoundManager::Instance().Play_Sound("footsteps");
	}
	else if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_S))
	{
		m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_UP);
		m_pPlayer->GetRigidBody()->velocity.y += 12.5f;
		//SoundManager::Instance().Play_Sound("footsteps");
	}
	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_A))
	{
		m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_LEFT);
		m_pPlayer->GetRigidBody()->velocity.x -= 12.5f;
		//SoundManager::Instance().Play_Sound("footsteps");
	}
	else if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_D))
	{
		m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_RIGHT);
		m_pPlayer->GetRigidBody()->velocity.x += 12.5f;
		//SoundManager::Instance().Play_Sound("footsteps");
	}
	if (EventManager::Instance().MousePressed(1))
	{
		std::cout << "Mouse 1 Pressed" << std::endl;
		// If player attack radius is touching the enemy in any way, melee attack!
		//SoundManager::Instance().Play_Sound("swipe");
		for (auto enemy : m_pEnemyPool->GetPool()) {
			if (Util::GetClosestEdge(m_pPlayer->GetTransform()->position, enemy) <= m_pPlayer->GetRangeOfAttack()) {
				m_pPlayer->MeleeAttack();
				enemy->TakeDamage(m_pPlayer->GetDamage());
			}
		}

	}
	if (EventManager::Instance().MousePressed(3))
	{
		std::cout << "Mouse 2 Pressed" << std::endl;
		Torpedo* temp = new TorpedoFederation(5.0f, Util::Normalize({ EventManager::Instance().GetMousePosition() - m_pPlayer->GetTransform()->position }));
		temp->SetTorpedoType(PLAYER);
		m_pTorpedoPool->FireTorpedo(temp);
		m_pTorpedoPool->GetPool().back()->GetTransform()->position = m_pPlayer->GetTransform()->position; // Set the spawn point
		SoundManager::Instance().SetSoundVolume(50);
		SoundManager::Instance().PlaySoundFX("torpedo");
	}

	if (Game::Instance().GetDebugMode())
	{
		if (EventManager::Instance().KeyPressed(SDL_SCANCODE_K))
		{
				for (const auto enemy : m_pEnemyPool->GetPool())
				{
					enemy->TakeDamage(10); // enemy takes fixed dmg.
					if (enemy->GetEnemyType() == EnemyType::CLOSE_COMBAT)
					{
						dynamic_cast<CloseCombatEnemy*>(enemy)->GetTree()->GetEnemyHitNode()->SetHit(true);
					}
					else {
						dynamic_cast<RangedCombatEnemy*>(enemy)->GetTree()->GetEnemyHitNode()->SetHit(true);
					}
					std::cout << "Starship at" << enemy->GetHealth() << "%. " << std::endl;
				}

		}

		if (EventManager::Instance().KeyPressed(SDL_SCANCODE_R))
		{
			m_pEnemyPool->SpawnEnemy(new CloseCombatEnemy(this), EnemyType::CLOSE_COMBAT);
			m_pEnemyPool->SpawnEnemy(new RangedCombatEnemy(this), EnemyType::RANGED);
		}

		if (EventManager::Instance().KeyPressed(SDL_SCANCODE_P))
		{
			for (const auto enemy : m_pEnemyPool->GetPool())
			{
				enemy->SetHealth(100); // Enemy Sets health

				if (enemy->GetEnemyType() == EnemyType::CLOSE_COMBAT)
				{
					dynamic_cast<CloseCombatEnemy*>(enemy)->GetTree()->GetEnemyHitNode()->SetHit(false);
					dynamic_cast<CloseCombatEnemy*>(enemy)->GetTree()->GetPlayerDetectedNode()->SetPlayerDetected(false);
				}
				else { // if (enemy->GetType() == EnemyType::RANGED)
					dynamic_cast<RangedCombatEnemy*>(enemy)->GetTree()->GetEnemyHitNode()->SetHit(false);
					dynamic_cast<RangedCombatEnemy*>(enemy)->GetTree()->GetPlayerDetectedNode()->SetPlayerDetected(false);
				}

			}

		}
	}

	
	
}

void PlayScene::Start()
{
	// Set GUI Title
	m_guiTitle = "Assignment 4";
	TextureManager::Instance().Load("../Assets/sprites/background/grass.png", "grass");

	Game::Instance().SetDebugMode(true);

	// Setup a few more fields
	m_LOSMode = LOSMode::SHIP;
	m_pathNodeLOSDistance = 1000; // 1000px distance
	m_setPathNodeLOSDistance(m_pathNodeLOSDistance);

	// Add Game Objects
	//m_pBackground = new Background();
	//AddChild(m_pBackground, 0);

	m_pPlayer = new Player();
	m_pPlayer->GetTransform()->position = glm::vec2(550.0f, 300.0f);
	AddChild(m_pPlayer, 2);

	// Initializing the enemy pool + spawning in stuff!.
	m_pEnemyPool = new EnemyPool();
	AddChild(m_pEnemyPool, 2);
	m_pEnemyPool->SpawnEnemy(new CloseCombatEnemy(this), EnemyType::CLOSE_COMBAT);
	m_pEnemyPool->SpawnEnemy(new RangedCombatEnemy(this), EnemyType::RANGED);

	// Set the random enemy position!
	for (const auto enemy : m_pEnemyPool->GetPool())
	{
		std::cout << "redsg\n";
		enemy->GetTransform()->position = glm::vec2(rand() % 200 + 150, rand() % 300 + 200);
	}
	// Adding remaining Enemies label
	m_RemainingEnemiesLabel = new Label();
	m_RemainingEnemiesLabel->GetTransform()->position = glm::vec2(50.0f, 50.0f);
	m_RemainingEnemiesLabel->SetColour({ 255,0,0,255 });
	AddChild(m_RemainingEnemiesLabel);


	m_pTorpedoPool = new TorpedoPool();
	AddChild(m_pTorpedoPool, 2);

	// Add Obstacles
	BuildObstaclePool();

	// Setup the Grid
	m_isGridEnabled = false;
	m_buildGrid();
	m_toggleGrid(m_isGridEnabled);


	// Preload Sounds

	SoundManager::Instance().Load("../Assets/Audio/yay.ogg", "yay", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/thunder.ogg", "thunder", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/torpedo.ogg", "torpedo", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/torpedo_k.ogg", "torpedo_k", SoundType::SOUND_SFX);


	// Preload music
	SoundManager::Instance().Load("../Assets/Audio/Klingon.mp3", "klingon", SoundType::SOUND_MUSIC);
	SoundManager::Instance().SetMusicVolume(16);

	// Play Music
	SoundManager::Instance().PlayMusic("klingon");

	


	ImGuiWindowFrame::Instance().SetGuiFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::SpawnEnemyTorpedo(Agent* enemyShooting)
{
	// Set Spawn Point (ront of our d7
	glm::vec2 spawn_point = enemyShooting->GetTransform()->position + enemyShooting->GetCurrentDirection() * 30.0f;

	// Set the direction of the Torpedo (normalized)
	glm::vec2 torpedo_direction = Util::Normalize(m_pPlayer->GetTransform()->position - spawn_point);

	// Spawn the torpedo
	Torpedo* temp = new TorpedoKlingon(5.0f, torpedo_direction);
	temp->SetTorpedoType(ENEMY);
	m_pTorpedoPool->FireTorpedo(temp);
	m_pTorpedoPool->GetPool().back()->GetTransform()->position = spawn_point; // Set the initial position of the torpedo to the spawn point
	SoundManager::Instance().PlaySoundFX("torpedo_k");
}

Player* PlayScene::GetTarget() const
{
	return m_pPlayer;
}

std::vector<PathNode*> PlayScene::GetGrid() const
{
	return m_pGrid;
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
		m_toggleGrid(m_isGridEnabled);
	}

	ImGui::Separator();

	static int LOS_mode = static_cast<int>(m_LOSMode);
	ImGui::Text("Path Node LOS");
	ImGui::RadioButton("Target", &LOS_mode, static_cast<int>(LOSMode::TARGET)); ImGui::SameLine();
	ImGui::RadioButton("StarShip", &LOS_mode, static_cast<int>(LOSMode::SHIP)); ImGui::SameLine();
	ImGui::RadioButton("Both Target & StarShip", &LOS_mode, static_cast<int>(LOSMode::BOTH));

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

	// allow the ship to rotate
	//static int angle;
	//if(ImGui::SliderInt("Ship Direction", &angle, -360, 360))
	//{
	//	m_pStarShip->SetCurrentHeading(static_cast<float>(angle));
	//}

	ImGui::Separator();

	// Target Properties
	static int targetPosition[] = { static_cast<int>(m_pPlayer->GetTransform()->position.x),
		static_cast<int>(m_pPlayer->GetTransform()->position.y) };
	if (ImGui::SliderInt2("Target Position", targetPosition, 0, 800))
	{
		m_pPlayer->GetTransform()->position.x = static_cast<float>(targetPosition[0]);
		m_pPlayer->GetTransform()->position.y = static_cast<float>(targetPosition[1]);
	}

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

void PlayScene::CheckCollision()
{
	for (const auto projectile : m_pTorpedoPool->GetPool())
	{
		bool canTorpedoHitEnemy;
		bool canTorpedoHitPlayer;

		switch (projectile->GetTorpedoType())
		{
		case PLAYER:
			for (const auto enemy : m_pEnemyPool->GetPool())
			{
				if (projectile->GetRigidBody()->isColliding)
				{
					canTorpedoHitEnemy = false;
				}
				else { canTorpedoHitEnemy = true; }

				if (CollisionManager::AABBCheck(enemy, projectile))
				{
					if (canTorpedoHitEnemy)
					{
						projectile->SetDeleteMe(true);
						enemy->TakeDamage(projectile->GetDamage());
					}
				}
			}
				break;
		case ENEMY:
			if (projectile->GetRigidBody()->isColliding)
			{
				canTorpedoHitPlayer = false;
			}
			else { canTorpedoHitPlayer = true; }

			if (CollisionManager::AABBCheck(m_pPlayer, projectile))
			{
				if (canTorpedoHitPlayer)
				{
					projectile->SetDeleteMe(true);
					m_pPlayer->TakeDamage(projectile->GetDamage());
				}
			}
			break;
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
				if(obstacle->GetType()==GameObjectType::DESTRUCT_OBSTACLE)
				{
					dynamic_cast<DestructibleObstacle*>(obstacle)->TakeDamage(proj->GetDamage());
				}
			}
		}
	}
}

void PlayScene::BuildObstaclePool()
{
	std::ifstream inFile("../Assets/data/obstacles.txt");

	while(!inFile.eof())
	{
		std::cout << "Obstacle " << std::endl;
		float x, y, w, h;
		int b; // declare variables the same as how the file is organized
		inFile >> x >> y >> w >> h >> b; // read data from file line by line
		auto obstacle = (b == 1) ? new DestructibleObstacle(): new Obstacle();
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
				&& (display_object->GetType() != GameObjectType::TARGET))
			{
				const auto agent_to_object_distance = Util::GetClosestEdge(agent->GetTransform()->position, display_object);

				if (agent_to_object_distance > agent_to_range) { contact_list.push_back(display_object); } // target is out of range

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
		for (const auto enemy : m_pEnemyPool->GetPool())
		{
			const bool LOSWithStarShip = m_checkPathNodeLOS(path_node, enemy);
			const bool LOSWithTarget = m_checkPathNodeLOS(path_node, m_pPlayer);
			path_node->SetHasLOS(LOSWithStarShip && LOSWithTarget, glm::vec4(0, 1, 1, 1));
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
