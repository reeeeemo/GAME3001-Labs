#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

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

	if (m_bDebugView)
	{
		Util::DrawCircle(m_pTarget->GetTransform->position, m_pTarget->GetWidth() * 0.5f);

		if (m_pStarship->IsEnabled())
		{
			Util::DrawRect(m_pStarship->GetTransform()->position - glm::vec2(m_pStarship->GetWidth() * 0.5, m_pStarship->GetHeight() * 0.5), m_pStarship->GetWidth(), m_pStarship->GetHeight());
		}
	}

	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	UpdateDisplayList();

	if (m_pStarship->IsEnabled())
	{
		CollisionManager::CircleAABBCheck(m_pTarget, m_pStarship);
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
	m_bDebugView = false;

	// Add Target to the Scene
	m_pTarget = new Target(); // Instantiate an object of type Target
	m_pTarget->GetTransform()->position = glm::vec2(500.0f, 100.0f);
	AddChild(m_pTarget);

	// Add the Starship to the screen
	m_pStarship = new Starship();
	m_pStarship->GetTransform()->position = glm::vec2(100.0f, 400.0f);
	m_pStarship->SetTargetPosition(m_pTarget->GetTransform()->position);
	m_pStarship->SetCurrentDirection(glm::vec2(1.0f, 0.0f)); // Looking right
	m_pStarship->SetEnabled(false);

	AddChild(m_pStarship);

	ImGuiWindowFrame::Instance().SetGuiFunction(std::bind(&PlayScene::GUI_Function, this));
	
}
	
void PlayScene::GUI_Function()
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();

	
	ImGui::Begin("Lab 2", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

	ImGui::Separator();

	static bool toggleDebug = false;
	if (ImGui::Checkbox("Toggle Debug", &toggleDebug))
	{
		m_bDebugView = toggleDebug;
	}

	ImGui::Separator();

	static float position[2] = { m_pTarget->GetTransform()->position.x, m_pTarget->GetTransform()->position.y};
	if (ImGui::SliderFloat2("Target Position", position, 0.0f, 800.0f)) {
		m_pTarget->GetTransform()->position = glm::vec2(position[0], position[1]);
		m_pStarship->SetTargetPosition(m_pTarget->GetTransform()->position);
	}

	ImGui::Separator();

	static bool toggleSeek = m_pStarship->IsEnabled();
	if (ImGui::Checkbox("Toggle Seek", &toggleSeek))
	{
		m_pStarship->SetEnabled(toggleSeek);
	}

	ImGui::Separator();
	static float speed = m_pStarship->GetMaxSpeed();
	if(ImGui::SliderFloat("Max Speed", &speed, 0.0f, 200.0f))
	{
		m_pStarship->SetMaxSpeed(speed);
	}

	ImGui::Separator();
	static float acceleration = m_pStarship->GetAccelerationRate();
	if (ImGui::SliderFloat("Acceleration Rate", &acceleration, 0.0f, 50.0f))
	{
		m_pStarship->SetAccelerationRate(acceleration);
	}

	ImGui::Separator();
	static float turn_rate = m_pStarship->GetTurnRate();
	if (ImGui::SliderFloat("Turn Rate", &turn_rate, 0.0f, 20.0f))
	{
		m_pStarship->SetTurnRate(turn_rate);
	}

	ImGui::Separator();

	if(ImGui::Button("Reset"))
	{
		// Reset Ship's position
		m_pStarship->GetTransform()->position = glm::vec2(100.0f, 400.0f);
		// Reset Target's position
		m_pTarget->GetTransform()->position = glm::vec2(500.0f, 100.0f);
		// Reset current Heading (orientation), velocity, and acceleration
		m_pStarship->SetCurrentHeading(0.0f);
		m_pStarship->GetRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
		m_pStarship->GetRigidBody()->acceleration = m_pStarship->GetCurrentDirection() * m_pStarship->GetAccelerationRate();

		m_pStarship->SetTargetPosition(m_pTarget->GetTransform()->position);
	}
	ImGui::End();
}
