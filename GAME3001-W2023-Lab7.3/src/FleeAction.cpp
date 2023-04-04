#include "FleeAction.h"

#include <iostream>

FleeAction::FleeAction()
{
	m_name = "Flee Action";
}

FleeAction::~FleeAction()
= default;

void FleeAction::Action()
{
	if (GetAgent()->GetActionState() != ActionState::FLEE)
	{
		std::cout << "Performing " << m_name << std::endl;
		GetAgent()->SetActionState(ActionState::FLEE);
	}
}
