#include "MoveToPlayerAction.h"

#include <iostream>

MoveToPlayerAction::MoveToPlayerAction()
{
	m_name = "Move to Player Action";
}

MoveToPlayerAction::~MoveToPlayerAction()
= default;

void MoveToPlayerAction::Action()
{
	if (GetAgent()->GetActionState() != ActionState::MOVE_TO_PLAYER)
	{
		std::cout << "Performing " << m_name << std::endl;
		GetAgent()->SetActionState(ActionState::MOVE_TO_PLAYER);
	}
}
