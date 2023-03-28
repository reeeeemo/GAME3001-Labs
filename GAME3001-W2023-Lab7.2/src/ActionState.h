#pragma once
#ifndef __ACTION_STATE__
#define __ACTION_STATE__
enum class ActionState
{
	NO_ACTION = -1,
	ATTACK,
	MOVE_TO_LOS,
	MOVE_TO_PLAYER,
	PATROL
};
#endif /* defined (__ACTION_STATE__)*/