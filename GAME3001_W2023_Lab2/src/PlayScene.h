#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Plane.h"
#include "Target.h"
#include "Starship.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene() override;

	// Scene LifeCycle Functions
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;
	virtual void HandleEvents() override;
	virtual void Start() override;
private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	
	// Scene Game Objects
	Target* m_pTarget;
	Starship* m_pStarship;
	glm::vec2 m_mousePosition;

	bool m_bDebugView;

};

#endif /* defined (__PLAY_SCENE__) */