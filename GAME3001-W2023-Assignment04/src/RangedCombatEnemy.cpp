#include "RangedCombatEnemy.h"

#include "Game.h"
#include "TextureManager.h"
#include "Util.h"
#include "MoveToLOSAction.h"
#include "MoveToPlayerAction.h"
#include "PatrolAction.h"
#include "EventManager.h"
#include "FleeAction.h"
#include "MoveToCoverAction.h"
#include "MoveToRange.h"
#include "WaitBehindCoverAction.h"
#include "PlaceholderSprite.h"

RangedCombatEnemy::RangedCombatEnemy(Scene* scene) :  m_fireCounter(0), m_fireCounterMax(60)
{				
	TextureManager::Instance().LoadSpriteSheet("../Assets/sprites/Player/turtle_sheet.txt",
		"../Assets/sprites/Player/turtle_sheet.png", "turtle");

	SetMaxSpeed(20.0f);
	SetTurnRate(5.0f);
	SetAccelerationRate(2.0f);
	SetStartPos(glm::vec2(300.0f, 500.0f));
	SetScene(scene);
	SetSprite(new PlaceholderSprite);
	SetFleePos(glm::vec2(rand() % 800 + 800, rand() % 600 + 600));

	SetMaxRange(350);
	SetMinRange(100);

	GetSprite()->SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("turtle"));

	SetHealth(100);

	const auto size = TextureManager::Instance().GetTextureSize("turtle");
	SetWidth(static_cast<int>(size.x));
	SetHeight(static_cast<int>(size.y));
	GetTransform()->position = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->bounds = glm::vec2(GetWidth(), GetHeight());
	GetRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->isColliding = false;
	setIsCentered(true);
	SetType(GameObjectType::AGENT);

	
	SetCurrentHeading(0.0f); // Current facing angle
	SetLOSDistance(1000.0f);
	SetWhiskerAngle(45.0f);
	SetLOSColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Default LOS Colour = Red

	// New for Lab 7.1
	SetActionState(ActionState::NO_ACTION);
	buildPatrolPath();

	// New for Lab 7.2
	SetTree(new DecisionTree(this));  // Create a new Tree - AI Brain
	m_buildTree();
	GetTree()->Display();
	BuildAnimations();
}

RangedCombatEnemy::~RangedCombatEnemy()
= default;

void RangedCombatEnemy::Draw()
{
	std::string current_anim = "";
	// Draw the enemy based on the animation state.
	switch (GetAnimationState())
	{
	case EnemyAnimationState::ENEMY_IDLE_LEFT:
		current_anim = "idle";
		TextureManager::Instance().PlayAnimation("turtle", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		
		break;
	case EnemyAnimationState::ENEMY_IDLE_RIGHT:
		current_anim = "idle";
		TextureManager::Instance().PlayAnimation("turtle", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case EnemyAnimationState::ENEMY_RUN_LEFT:
		current_anim = "run";
		TextureManager::Instance().PlayAnimation("turtle", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_RUN_RIGHT:
		current_anim = "run";
		TextureManager::Instance().PlayAnimation("turtle", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case EnemyAnimationState::ENEMY_RUN_DOWN:
		current_anim = "run_back";
		TextureManager::Instance().PlayAnimation("turtle", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_RUN_UP:
		current_anim = "run_front";
		TextureManager::Instance().PlayAnimation("turtle", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_DAMAGE:
		current_anim = "damaged";
		TextureManager::Instance().PlayAnimation("turtle", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_DEAD:
		current_anim = "dead";
		TextureManager::Instance().PlayAnimation("turtle", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	}

	SetWidth(GetSprite()->GetAnimation(current_anim).frames[0].w);
	SetHeight(GetSprite()->GetAnimation(current_anim).frames[0].h);

	// Draw the health based on the amount the enemy has
	if (GetHealth() > 0)
	{
		Util::DrawFilledRect(GetTransform()->position - glm::vec2((GetHealth() / GetMaxHealth() * 100) / 2, 60.0f), GetHealth() / GetMaxHealth() * 100.0f, 10.0f, glm::vec4(0, 1.0f, 0, 1.0f));
	} else
	{
		//Util::DrawFilledRect(GetTransform()->position - glm::vec2( 0.0f, 60.0f), GetHealth() / GetMaxHealth() * 100.0f, 10.0f, glm::vec4(0, 1.0f, 0, 1.0f));
	}

	if (EventManager::Instance().IsIMGUIActive()) 
	{
		// draw the LOS Line
		Util::DrawLine(GetTransform()->position + GetCurrentDirection() * 0.5f * static_cast<float>(GetWidth()),
			GetMiddleLOSEndPoint(), GetLOSColour());
	}

	// If we are in debug mode, draw the collider rect.
	if (Game::Instance().GetDebugMode())
	{
		Util::DrawRect(GetTransform()->position -
			glm::vec2(this->GetWidth() * 0.5f, this->GetHeight() * 0.5f),
			this->GetWidth(), this->GetHeight());
	}
}

void RangedCombatEnemy::Update()
{
	// Determine which action to perform
	GetTree()->MakeDecision();
	if (GetTree()->GetEnemyHitNode()->GetHit())
	{
		SetAnimationState(EnemyAnimationState::ENEMY_DAMAGE);
	}
	if (GetHealth() <= 0)
	{
		SetAnimationState(EnemyAnimationState::ENEMY_DEAD);
	}
}

void RangedCombatEnemy::Clean()
{
}



void RangedCombatEnemy::Attack()
{
	auto scene = dynamic_cast<PlayScene*>(GetScene());

	if (GetActionState() != ActionState::ATTACK) {
		// Initialize
		SetActionState(ActionState::ATTACK);
	}
	// New for lab 8
	// Need to get the target object from Play Scene
	glm::vec2 target_direction = Util::Normalize(scene->GetTarget()->GetTransform()->position - GetTransform()->position);
	LookWhereYoureGoing(target_direction, false);

	// Wait for a number of frames before firing = frame delay
	if (m_fireCounter++ % m_fireCounterMax == 0)
	{
		scene->SpawnEnemyTorpedo(this);
	}
}

void RangedCombatEnemy::BuildAnimations()
{
	Animation idle_animation = Animation();

	idle_animation.name = "idle";
	idle_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("idle"));

	GetSprite()->SetAnimation(idle_animation);

	Animation run_animation = Animation();

	run_animation.name = "run";
	run_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run1"));
	run_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run2"));

	GetSprite()->SetAnimation(run_animation);

	Animation run_back_animation = Animation();

	run_back_animation.name = "run_back";
	run_back_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run_back1"));
	run_back_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run_back2"));

	GetSprite()->SetAnimation(run_back_animation);

	Animation run_front_animation = Animation();

	run_front_animation.name = "run_front";
	run_front_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run_front1"));
	run_front_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run_front2"));

	GetSprite()->SetAnimation(run_front_animation);

	Animation death_animation = Animation();

	death_animation.name = "dead";
	death_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("dead"));

	GetSprite()->SetAnimation(death_animation);

	Animation damage_animation = Animation();

	damage_animation.name = "damaged";
	damage_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("damaged"));

	GetSprite()->SetAnimation(damage_animation);
}

//void RangedCombatEnemy::m_move()
//{
//	Seek(); // Get our target for this frame
//
//	//                      final Position  Position Term   Velocity      Acceleration Term
//	// Kinematic Equation-> Pf            = Pi +            Vi * (time) + (0.5) * Ai * (time * time)
//
//	const float dt = Game::Instance().GetDeltaTime();
//
//	// accessing the position Term
//	const glm::vec2 initial_position = GetTransform()->position;
//
//	// compute the velocity Term
//	const glm::vec2 velocity_term = GetRigidBody()->velocity * dt;
//
//	// compute the acceleration Term
//	const glm::vec2 acceleration_term = GetRigidBody()->acceleration * 0.5f; // * dt * dt
//
//
//	// compute the new position
//	glm::vec2 final_position = initial_position + velocity_term + acceleration_term;
//
//	GetTransform()->position = final_position;
//
//	// add our acceleration to velocity
//	GetRigidBody()->velocity += GetRigidBody()->acceleration;
//
//	// clamp our velocity at max speed
//	GetRigidBody()->velocity = Util::Clamp(GetRigidBody()->velocity, GetMaxSpeed());
//
//	float xDir = abs(GetCurrentDirection().x);
//	float yDir = abs(GetCurrentDirection().y);
//
//	if (xDir > yDir && GetCurrentDirection().x > 0)
//	{
//		SetAnimationState(EnemyAnimationState::ENEMY_RUN_RIGHT);
//	}
//	else if (xDir > yDir && GetCurrentDirection().x < 0)
//	{
//		SetAnimationState(EnemyAnimationState::ENEMY_RUN_LEFT);
//	}
//
//	if (yDir > xDir && GetCurrentDirection().y > 0)
//	{
//		SetAnimationState(EnemyAnimationState::ENEMY_RUN_UP);
//	}
//	else if (yDir > xDir && GetCurrentDirection().y < 0)
//	{
//		SetAnimationState(EnemyAnimationState::ENEMY_RUN_DOWN);
//	}
//
//	if (Util::Magnitude(GetRigidBody()->velocity) <= 5)
//	{
//		if (GetAnimationState() != EnemyAnimationState::ENEMY_IDLE_LEFT || GetAnimationState() != EnemyAnimationState::ENEMY_IDLE_RIGHT)
//		{
//			if (GetAnimationState() == EnemyAnimationState::ENEMY_RUN_LEFT)
//			{
//				SetAnimationState(EnemyAnimationState::ENEMY_IDLE_LEFT);
//			}
//			else
//			{
//				SetAnimationState(EnemyAnimationState::ENEMY_IDLE_RIGHT);
//			}
//		}
//	}
//}


void RangedCombatEnemy::m_buildTree()
{
	// Create and add the root node - Health Condition
	GetTree()->SetEnemyHealthNode(new EnemyHealthCondition(this, true));

	GetTree()->GetTree().push_back(GetTree()->GetEnemyHealthNode());

	m_buildLeftTree();
	m_buildRightTree();
}

void RangedCombatEnemy::m_buildLeftTree()
{
	// Left Subtree Level 1 -> Flee Action
	TreeNode* fleeNode = GetTree()->AddNode(GetTree()->GetEnemyHealthNode(), new FleeAction(this), TreeNodeType::LEFT_TREE_NODE);
	GetTree()->GetTree().push_back(fleeNode);
}

void RangedCombatEnemy::m_buildRightTree()
{
	// Right Subtree Level 1 -> Enemy Hit Condition
	GetTree()->SetEnemyHitNode(new EnemyHitCondition(this, false));
	GetTree()->AddNode(GetTree()->GetEnemyHealthNode(), GetTree()->GetEnemyHitNode(), TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(GetTree()->GetEnemyHitNode());

	// Right Left Subtree Level 2 -> Player Detected Condition
	GetTree()->SetPlayerDetectedNode(new PlayerDetectedCondition(this));
	GetTree()->AddNode(GetTree()->GetEnemyHitNode(), GetTree()->GetPlayerDetectedNode(), TreeNodeType::LEFT_TREE_NODE);

	// Right Subtree Level 2 -> LOS Condition
	const auto LOSNodeRight = new LOSCondition(this);
	GetTree()->AddNode(GetTree()->GetEnemyHitNode(), LOSNodeRight, TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(LOSNodeRight);

	// Left Left Subtree Level 3 -> Patrol Action
	TreeNode* patrolNode = GetTree()->AddNode(GetTree()->GetPlayerDetectedNode(), new PatrolAction(this), TreeNodeType::LEFT_TREE_NODE);
	GetTree()->GetTree().push_back(patrolNode);

	// Left Right Subtree Level 3 -> LOS Condition
	const auto LOSNodeLeft = new LOSCondition(this);
	GetTree()->AddNode(GetTree()->GetPlayerDetectedNode(), LOSNodeLeft, TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(LOSNodeLeft);

	// Right Left Subtree Level 3 -> Wait Behind Cover Action
	TreeNode* waitBehindCoverNode = GetTree()->AddNode(LOSNodeRight, new WaitBehindCoverAction(this), TreeNodeType::LEFT_TREE_NODE);
	GetTree()->GetTree().push_back(waitBehindCoverNode);

	// Right Right Subtree Level 3 -> Move to Cover Action
	TreeNode* moveToCoverNode = GetTree()->AddNode(LOSNodeRight, new MoveToCoverAction(this), TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(moveToCoverNode);

	// Left Left Subtree Level 4 -> Move to LOS Action
	TreeNode* moveToLOS = GetTree()->AddNode(LOSNodeLeft, new MoveToLOSAction(this), TreeNodeType::LEFT_TREE_NODE);
	GetTree()->GetTree().push_back(moveToLOS);

	// Left Right Subtree Level 4 -> Ranged Combat Condition
	GetTree()->SetRangedCombatNode(new RangedCombatCondition(this));
	GetTree()->AddNode(LOSNodeLeft, GetTree()->GetRangedCombatNode(), TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(GetTree()->GetRangedCombatNode());

	// Left Left Subtree Level 5 -> Move to Range Action
	TreeNode* moveToRangeNode = GetTree()->AddNode(GetTree()->GetRangedCombatNode(), new MoveToRangeAction(this), TreeNodeType::LEFT_TREE_NODE);
	//dynamic_cast<ActionNode*>(moveToRangeNode)->SetAgent(this);
	GetTree()->GetTree().push_back(moveToRangeNode);

	// Left Right Subtree Level 5 -> Attack Action
	TreeNode* attackNode = GetTree()->AddNode(GetTree()->GetRangedCombatNode(), new AttackAction(this), TreeNodeType::RIGHT_TREE_NODE);
	//dynamic_cast<ActionNode*>(attackNode)->SetAgent(this);
	GetTree()->GetTree().push_back(attackNode);
}
