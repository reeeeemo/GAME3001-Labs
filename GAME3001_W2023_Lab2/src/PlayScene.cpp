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

	// Add the Starship to the screen
	m_pStarship = new Starship();
	m_pStarship->GetTransform()->position = glm::vec2(400.0f, 300.0f);
	AddChild(m_pStarship);

	ImGuiWindowFrame::Instance().SetGuiFunction(std::bind(&PlayScene::GUI_Function, this));

}
	
void PlayScene::GUI_Function() const
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();

	ImGui::Begin("Lab 2", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

	/*static float position[2] = {m_pTarget->GetTransform()->position.x, m_pTarget->GetTransform()->position.y};
	if (ImGui::SliderFloat2("Target Position", position, 0.0f, 800.0f)) {
		m_pTarget->GetTransform()->position = glm::vec2(position[0], position[1]);
	}*/ // Causing a breakpoint??? Figure this out.
	
	ImGui::End();
}
