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
	
	ImGui::End();
}
