#include "CloseCombatEnemy.h"

#include "Game.h"
#include "TextureManager.h"
#include "Util.h"
#include "MoveToLOSAction.h"
#include "MoveToPlayerAction.h"
#include "PatrolAction.h"
#include "EventManager.h"

CloseCombatEnemy::CloseCombatEnemy(Scene* scene) : m_maxSpeed(20.0f),
m_turnRate(5.0f), m_accelerationRate(2.0f), m_startPosition(glm::vec2(300.0f, 500.0f)), m_pScene(scene)
{
	TextureManager::Instance().LoadSpriteSheet("../Assets/sprites/Player/turtle_sheet.txt",
		"../Assets/sprites/Player/turtle_sheet.png", "turtle");

	m_sprite = new PlaceholderSprite;

	SetHealth(100);

	m_sprite->SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("turtle"));

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
	SetLOSDistance(400.0f);
	SetWhiskerAngle(45.0f);
	SetLOSColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Default LOS Colour = Red

	// New for Lab 7.1
	SetActionState(ActionState::NO_ACTION);
	m_buildPatrolPath();

	// New for Lab 7.2
	m_tree = new DecisionTree(this); // Create a new Tree - AI Brain
	m_buildTree();
	m_tree->Display();
	BuildAnimations();
}

CloseCombatEnemy::~CloseCombatEnemy()
= default;

void CloseCombatEnemy::Draw()
{
	std::string current_anim = "";
	// Draw the enemy based on the animation state.
	switch (GetAnimationState())
	{
	case EnemyAnimationState::ENEMY_IDLE_LEFT:
		current_anim = "idle";
		TextureManager::Instance().PlayAnimation("turtle", m_sprite->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);

		break;
	case EnemyAnimationState::ENEMY_IDLE_RIGHT:
		current_anim = "idle";
		TextureManager::Instance().PlayAnimation("turtle", m_sprite->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true, SDL_FLIP_VERTICAL);
		break;
	case EnemyAnimationState::ENEMY_RUN_LEFT:
		current_anim = "run";
		TextureManager::Instance().PlayAnimation("turtle", m_sprite->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_RUN_RIGHT:
		current_anim = "run";
		TextureManager::Instance().PlayAnimation("turtle", m_sprite->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true, SDL_FLIP_VERTICAL);
		break;
	case EnemyAnimationState::ENEMY_RUN_DOWN:
		current_anim = "run_back";
		TextureManager::Instance().PlayAnimation("turtle", m_sprite->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_RUN_UP:
		current_anim = "run_front";
		TextureManager::Instance().PlayAnimation("turtle", m_sprite->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_DAMAGE:
		current_anim = "damaged";
		TextureManager::Instance().PlayAnimation("turtle", m_sprite->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_DEAD:
		current_anim = "dead";
		TextureManager::Instance().PlayAnimation("turtle", m_sprite->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	}

	SetWidth(m_sprite->GetAnimation(current_anim).frames[0].w);
	SetHeight(m_sprite->GetAnimation(current_anim).frames[0].h);

	// Draw the health based on the amount the enemy has
	if (GetHealth() > 0)
	{
		Util::DrawFilledRect(GetTransform()->position - glm::vec2((GetHealth() / GetMaxHealth() * 100) / 2, 60.0f), GetHealth() / GetMaxHealth() * 100.0f, 10.0f, glm::vec4(0, 1.0f, 0, 1.0f));
	}
	else
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

void CloseCombatEnemy::Update()
{
	// Determine which action to perform
	m_tree->MakeDecision();
	if(GetHealth()<=0)
	{
		Clean();
	}
}

void CloseCombatEnemy::Clean()
{
}

float CloseCombatEnemy::GetMaxSpeed() const
{
	return m_maxSpeed;
}

float CloseCombatEnemy::GetTurnRate() const
{
	return m_turnRate;
}

float CloseCombatEnemy::GetAccelerationRate() const
{
	return m_accelerationRate;
}

glm::vec2 CloseCombatEnemy::GetDesiredVelocity() const
{
	return m_desiredVelocity;
}

void CloseCombatEnemy::SetMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void CloseCombatEnemy::SetTurnRate(const float angle)
{
	m_turnRate = angle;
}

void CloseCombatEnemy::SetAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void CloseCombatEnemy::SetDesiredVelocity(const glm::vec2 target_position)
{
	//SetTargetPosition(target_position);
	m_desiredVelocity = Util::Normalize(target_position - GetTransform()->position);
	//GetRigidBody()->velocity = m_desiredVelocity - GetRigidBody()->velocity;
}

void CloseCombatEnemy::Seek()
{
	// Find next waypoint if within 10px of the current waypoint
	if (!m_movingTowardsPlayer) {
		if (Util::Distance(m_patrolPath[m_wayPoint], GetTransform()->position) < 10)
		{
			// Check to see if you are at the last point in the path
			if (++m_wayPoint == m_patrolPath.size())
			{
				// if so.. reset
				m_wayPoint = 0;
			}
			SetTargetPosition(m_patrolPath[m_wayPoint]);
		}
	}
	
		SetDesiredVelocity(GetTargetPosition());

		const glm::vec2 steering_direction = GetDesiredVelocity() - GetCurrentDirection();

		LookWhereYoureGoing(steering_direction, true);

		GetRigidBody()->acceleration = GetCurrentDirection() * GetAccelerationRate();
}

void CloseCombatEnemy::LookWhereYoureGoing(const glm::vec2 target_direction, bool direction_hack = true)
{
	const float direction_axis = direction_hack ? 90.0f : 0.0f;
	float target_rotation = Util::SignedAngle(GetCurrentDirection(), target_direction) - direction_axis;

	last_rotation = target_rotation;

	if (target_rotation < 0)
	{
		target_rotation += 180.0f;
	}
	if (target_rotation < 184 && target_rotation > 176)
	{
		target_rotation = last_rotation;
	}

	const float turn_sensitivity = 3.0f;

	if (GetCollisionWhiskers()[0] || GetCollisionWhiskers()[1] || GetCollisionWhiskers()[2])
	{
		target_rotation += GetTurnRate() * turn_sensitivity;
	}
	else if (GetCollisionWhiskers()[3] || GetCollisionWhiskers()[4])
	{
		target_rotation -= GetTurnRate() * turn_sensitivity;
	}

	SetCurrentHeading(Util::LerpUnclamped(GetCurrentHeading(),
		GetCurrentHeading() + target_rotation, GetTurnRate() * Game::Instance().GetDeltaTime()));

	UpdateWhiskers(GetWhiskerAngle());
}

void CloseCombatEnemy::Reset()
{
	GetTransform()->position = m_startPosition;
}

void CloseCombatEnemy::Patrol()
{
	if (GetActionState() != ActionState::PATROL) {
		// Initialize
		SetActionState(ActionState::PATROL);
	}
	m_movingTowardsPlayer = false;
	m_move();
}

void CloseCombatEnemy::MoveToLOS()
{
	auto scene = dynamic_cast<PlayScene*>(m_pScene);

	//glm::vec2 target_direction = Util::Normalize(scene->GetTarget()->GetLOSDistance() - GetTransform()->position);
	SetTargetPosition({ scene->GetTarget()->GetTransform()->position.x, scene->GetTarget()->GetTransform()->position.y - GetLOSDistance()});
	//LookWhereYoureGoing(target_direction);
	m_move();
}

void CloseCombatEnemy::MoveToPlayer()
{
	auto scene = dynamic_cast<PlayScene*>(m_pScene);
	m_movingTowardsPlayer = true;

	if (GetActionState() != ActionState::MOVE_TO_PLAYER) {
		// Initialize
		SetActionState(ActionState::MOVE_TO_PLAYER);
	}
	// TODO: setup another action to take when moving to the player.
	//glm::vec2 target_direction = Util::Normalize(scene->GetTarget()->GetTransform()->position - GetTransform()->position);
	SetTargetPosition(scene->GetTarget()->GetTransform()->position);
	//LookWhereYoureGoing(target_direction);
	m_move();
}

void CloseCombatEnemy::Attack()
{

}


void CloseCombatEnemy::m_move()
{
	Seek(); // Get our target for this frame

	//                      final Position  Position Term   Velocity      Acceleration Term
	// Kinematic Equation-> Pf            = Pi +            Vi * (time) + (0.5) * Ai * (time * time)

	const float dt = Game::Instance().GetDeltaTime();

	// accessing the position Term
	const glm::vec2 initial_position = GetTransform()->position;

	// compute the velocity Term
	const glm::vec2 velocity_term = GetRigidBody()->velocity * dt;

	// compute the acceleration Term
	const glm::vec2 acceleration_term = GetRigidBody()->acceleration * 0.5f; // * dt * dt


	// compute the new position
	glm::vec2 final_position = initial_position + velocity_term + acceleration_term;

	GetTransform()->position = final_position;

	// add our acceleration to velocity
	GetRigidBody()->velocity += GetRigidBody()->acceleration;

	// clamp our velocity at max speed
	GetRigidBody()->velocity = Util::Clamp(GetRigidBody()->velocity, GetMaxSpeed());

	float xDir = abs(GetCurrentDirection().x);
	float yDir = abs(GetCurrentDirection().y);

	if (xDir > yDir && GetCurrentDirection().x > 0)
	{
		SetAnimationState(EnemyAnimationState::ENEMY_RUN_RIGHT);
	}
	else if (xDir > yDir && GetCurrentDirection().x < 0)
	{
		SetAnimationState(EnemyAnimationState::ENEMY_RUN_LEFT);
	}

	if (yDir > xDir && GetCurrentDirection().y > 0)
	{
		SetAnimationState(EnemyAnimationState::ENEMY_RUN_UP);
	}
	else if (yDir > xDir && GetCurrentDirection().y < 0)
	{
		SetAnimationState(EnemyAnimationState::ENEMY_RUN_DOWN);
	}

	if (Util::Magnitude(GetRigidBody()->velocity) <= 5)
	{
		if (GetAnimationState() != EnemyAnimationState::ENEMY_IDLE_LEFT || GetAnimationState() != EnemyAnimationState::ENEMY_IDLE_RIGHT)
		{
			if (GetAnimationState() == EnemyAnimationState::ENEMY_RUN_LEFT)
			{
				SetAnimationState(EnemyAnimationState::ENEMY_IDLE_LEFT);
			}
			else
			{
				SetAnimationState(EnemyAnimationState::ENEMY_IDLE_RIGHT);
			}
		}
	}
}

void CloseCombatEnemy::m_buildPatrolPath()
{
	m_patrolPath.push_back(glm::vec2(760, 40)); // Top Right Corner node

	m_patrolPath.push_back(glm::vec2(760, 560)); // Bottom Right Corner node

	m_patrolPath.push_back(glm::vec2(40, 560)); // Bottom Left Corner node

	m_patrolPath.push_back(glm::vec2(40, 40)); // Top Left Corner node

	SetTargetPosition(m_patrolPath[m_wayPoint]);

}

void CloseCombatEnemy::m_buildTree()
{
	// Create and add the root node - Health Condition
	m_tree->SetEnemyHealthNode(new EnemyHealthCondition(this, true));

	m_tree->GetTree().push_back(m_tree->GetEnemyHealthNode());


	//Conditions

	// Left Subtree Level 1 -> Flee Action
	TreeNode* fleeNode = m_tree->AddNode(m_tree->GetEnemyHealthNode(), new FleeAction(this), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetTree().push_back(fleeNode);


	// Right Subtree Level 1 -> Enemy Hit Condition
	m_tree->SetEnemyHitNode(new EnemyHitCondition(this, false));
	m_tree->AddNode(m_tree->GetEnemyHealthNode(), m_tree->GetEnemyHitNode(), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetTree().push_back(m_tree->GetEnemyHitNode());

	// Right Left Subtree Level 2 -> Player Detected Condition
	m_tree->SetPlayerDetectedNode(new PlayerDetectedCondition(this));
	m_tree->AddNode(m_tree->GetEnemyHitNode(), m_tree->GetPlayerDetectedNode(), TreeNodeType::LEFT_TREE_NODE);

	// Right Subtree Level 2 -> LOS Condition
	const auto LOSNodeRight = new LOSCondition(this);
	m_tree->AddNode(m_tree->GetEnemyHitNode(), LOSNodeRight, TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetTree().push_back(LOSNodeRight);

	// Left Left Subtree Level 3 -> Patrol Action
	TreeNode* patrolNode = m_tree->AddNode(m_tree->GetPlayerDetectedNode(), new PatrolAction(this), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetTree().push_back(patrolNode);

	// Left Right Subtree Level 3 -> LOS Condition
	const auto LOSNodeLeft = new LOSCondition(this);
	m_tree->AddNode(m_tree->GetPlayerDetectedNode(), LOSNodeLeft, TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetTree().push_back(LOSNodeLeft);

	// Right Left Subtree Level 3 -> Wait Behind Cover Action
	TreeNode* waitBehindCoverNode = m_tree->AddNode(LOSNodeRight, new WaitBehindCoverAction(this), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetTree().push_back(waitBehindCoverNode);

	// Right Right Subtree Level 3 -> Move to Cover Action
	TreeNode* moveToCoverNode = m_tree->AddNode(LOSNodeRight, new MoveToCoverAction(this), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetTree().push_back(moveToCoverNode);

	// Left Left Subtree Level 4 -> Move to LOS Action
	TreeNode* moveToLOS = m_tree->AddNode(LOSNodeLeft, new MoveToLOSAction(this), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetTree().push_back(moveToLOS);

	// Left Right Subtree Level 4 -> Close Combat Condition
	m_tree->SetCloseCombatNode(new CloseCombatCondition(this));
	m_tree->AddNode(LOSNodeLeft, m_tree->GetCloseCombatNode(), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetTree().push_back(m_tree->GetCloseCombatNode());

	// Left Left Subtree Level 5 -> Move to Player Action
	TreeNode* moveToPlayer = m_tree->AddNode(m_tree->GetCloseCombatNode(), new MoveToPlayerAction(this), TreeNodeType::LEFT_TREE_NODE);
	//dynamic_cast<ActionNode*>(moveToRangeNode)->SetAgent(this);
	m_tree->GetTree().push_back(moveToPlayer);

	// Left Right Subtree Level 5 -> Attack Action
	TreeNode* attackNode = m_tree->AddNode(m_tree->GetCloseCombatNode(), new AttackAction(this), TreeNodeType::RIGHT_TREE_NODE);
	//dynamic_cast<ActionNode*>(attackNode)->SetAgent(this);
	m_tree->GetTree().push_back(attackNode);
}

DecisionTree* CloseCombatEnemy::GetTree() const
{
	return m_tree;
}

void CloseCombatEnemy::BuildAnimations()
{
	Animation idle_animation = Animation();

	idle_animation.name = "idle";
	idle_animation.frames.push_back(m_sprite->GetSpriteSheet()->GetFrame("idle"));

	m_sprite->SetAnimation(idle_animation);

	Animation run_animation = Animation();

	run_animation.name = "run";
	run_animation.frames.push_back(m_sprite->GetSpriteSheet()->GetFrame("run1"));
	run_animation.frames.push_back(m_sprite->GetSpriteSheet()->GetFrame("run2"));

	m_sprite->SetAnimation(run_animation);

	Animation run_back_animation = Animation();

	run_back_animation.name = "run_back";
	run_back_animation.frames.push_back(m_sprite->GetSpriteSheet()->GetFrame("run_back1"));
	run_back_animation.frames.push_back(m_sprite->GetSpriteSheet()->GetFrame("run_back2"));

	m_sprite->SetAnimation(run_back_animation);

	Animation run_front_animation = Animation();

	run_front_animation.name = "run_front";
	run_front_animation.frames.push_back(m_sprite->GetSpriteSheet()->GetFrame("run_front1"));
	run_front_animation.frames.push_back(m_sprite->GetSpriteSheet()->GetFrame("run_front2"));

	m_sprite->SetAnimation(run_front_animation);

	Animation death_animation = Animation();

	death_animation.name = "dead";
	death_animation.frames.push_back(m_sprite->GetSpriteSheet()->GetFrame("dead"));

	m_sprite->SetAnimation(death_animation);

	Animation damage_animation = Animation();

	damage_animation.name = "damaged";
	damage_animation.frames.push_back(m_sprite->GetSpriteSheet()->GetFrame("damaged"));

	m_sprite->SetAnimation(damage_animation);
}
