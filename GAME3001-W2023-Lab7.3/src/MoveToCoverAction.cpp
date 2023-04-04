#include "MoveToCoverAction.h"

#include <iostream>

MoveToCoverAction::MoveToCoverAction()
{
	m_name = "Move To Cover Action";
}

MoveToCoverAction::~MoveToCoverAction()
= default;

void MoveToCoverAction::Action()
{
	if (GetAgent()->GetActionState() != ActionState::MOVE_TO_COVER)
	{
		std::cout << "Performing " << m_name << std::endl;
		GetAgent()->SetActionState(ActionState::MOVE_TO_COVER);
	}
}
