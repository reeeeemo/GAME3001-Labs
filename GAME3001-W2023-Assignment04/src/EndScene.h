#pragma once
#ifndef __END_SCENE__
#define __END_SCENE__

#include "Scene.h"
#include "Label.h"
#include "Button.h"

class EndScene final : public Scene
{
public:
	EndScene(bool win);
	~EndScene() override;

	// Inherited via Scene
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;
	virtual void HandleEvents() override;
	virtual void Start() override;

private:
	Label* m_label{};

	Button* m_pRestartButton{};
	bool m_win;
};

#endif /* defined (__END_SCENE__) */