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

RangedCombatEnemy::RangedCombatEnemy(Scene* scene) : m_maxSpeed(20.0f), m_turnRate(5.0f), m_accelerationRate(2.0f), m_startPosition(glm::vec2(300.0f, 500.0f)),
m_pScene(scene), m_fireCounter(0), m_fireCounterMax(60)
{								
	TextureManager::Instance().Load("../Assets/textures/d7_small.png", "ranged_combat_enemy");

	const auto size = TextureManager::Instance().GetTextureSize("ranged_combat_enemy");
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
	m_buildPatrolPath();

	// New for Lab 7.2
	m_tree = new DecisionTree(this); // Create a new Tree - AI Brain
	m_buildTree();
	m_tree->Display();

	max_range = 350.0f;
	min_range = 250.0f;
}

RangedCombatEnemy::~RangedCombatEnemy()
= default;

void RangedCombatEnemy::Draw()
{
	// draw the StarShip
	TextureManager::Instance().Draw("ranged_combat_enemy", 
		GetTransform()->position, static_cast<double>(GetCurrentHeading()), 255, true);

	if (EventManager::Instance().IsIMGUIActive()) 
	{
		// draw the LOS Line
		Util::DrawLine(GetTransform()->position + GetCurrentDirection() * 0.5f * static_cast<float>(GetWidth()),
			GetMiddleLOSEndPoint(), GetLOSColour());
	}

	
}

float RangedCombatEnemy::GetMinRange() const
{
	return min_range;
}

float RangedCombatEnemy::GetMaxRange() const
{
	return max_range;
}

void RangedCombatEnemy::MoveToRange()
{
	auto scene = dynamic_cast<PlayScene*>(m_pScene);
	m_movingTowardsPlayer = true;

	if (GetActionState() != ActionState::MOVE_TO_RANGE) {
		// Initialize
		SetActionState(ActionState::MOVE_TO_RANGE);
	}
	// TODO: setup another action to take when moving to the player.
	for (const auto node : scene->GetGrid())
	{
		if (Util::Distance(scene->GetTarget()->GetTransform()->position, node->GetTransform()->position) >= GetMinRange() && node->HasLOS())
		{
			SetTargetPosition(node->GetTransform()->position);
		}
	}
	m_move();
}


void RangedCombatEnemy::Update()
{
	// Determine which action to perform
	m_tree->MakeDecision();
}

void RangedCombatEnemy::Clean()
{
}

float RangedCombatEnemy::GetMaxSpeed() const
{
	return m_maxSpeed;
}

float RangedCombatEnemy::GetTurnRate() const
{
	return m_turnRate;
}

float RangedCombatEnemy::GetAccelerationRate() const
{
	return m_accelerationRate;
}

glm::vec2 RangedCombatEnemy::GetDesiredVelocity() const
{
	return m_desiredVelocity;
}

void RangedCombatEnemy::SetMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void RangedCombatEnemy::SetTurnRate(const float angle)
{
	m_turnRate = angle;
}

void RangedCombatEnemy::SetAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void RangedCombatEnemy::SetDesiredVelocity(const glm::vec2 target_position)
{
	//SetTargetPosition(target_position);
	m_desiredVelocity = Util::Normalize(target_position - GetTransform()->position);
	//GetRigidBody()->velocity = m_desiredVelocity - GetRigidBody()->velocity;
}

void RangedCombatEnemy::Seek()
{
	// New for Lab 7.1
	// Find next waypoint if within 10px of the current waypoint

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


		SetDesiredVelocity(GetTargetPosition());

		const glm::vec2 steering_direction = GetDesiredVelocity() - GetCurrentDirection();

		LookWhereYoureGoing(steering_direction);

		GetRigidBody()->acceleration = GetCurrentDirection() * GetAccelerationRate();
}

void RangedCombatEnemy::LookWhereYoureGoing(const glm::vec2 target_direction, bool direction_hack)
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

	if(GetCollisionWhiskers()[0] || GetCollisionWhiskers()[1] || GetCollisionWhiskers()[2])
	{
		target_rotation += GetTurnRate() * turn_sensitivity;
	}
	else if(GetCollisionWhiskers()[3] || GetCollisionWhiskers()[4])
	{
		target_rotation -= GetTurnRate() * turn_sensitivity;
	}

	SetCurrentHeading(Util::LerpUnclamped(GetCurrentHeading(), 
		GetCurrentHeading() + target_rotation, GetTurnRate() * Game::Instance().GetDeltaTime()));

	UpdateWhiskers(GetWhiskerAngle());
}

void RangedCombatEnemy::Reset()
{
	GetTransform()->position = m_startPosition;
}

void RangedCombatEnemy::Patrol()
{
	if (GetActionState() != ActionState::PATROL) {
		// Initialize
		SetActionState(ActionState::PATROL);
	}
	m_move();
}

DecisionTree* RangedCombatEnemy::GetTree() const
{
	return m_tree;
}

void RangedCombatEnemy::Flee()
{
	if (GetActionState() != ActionState::FLEE) {
		// Initialize
		SetActionState(ActionState::FLEE);
	}
	// TODO: setup another action to take when moving to the player.
}

void RangedCombatEnemy::MoveToLOS()
{
	auto scene = dynamic_cast<PlayScene*>(m_pScene);

	if (GetActionState() != ActionState::MOVE_TO_LOS) {
		// Initialize
		SetActionState(ActionState::MOVE_TO_LOS);
	}

	m_movingTowardsPlayer = true;
	float distance = 1000.0f;
	PathNode* curNode = nullptr;
	for (PathNode* node : scene->GetGrid())
	{
		if (!node->HasLOS())
		{
			float temp = Util::Distance(node->GetTransform()->position, scene->GetTarget()->GetTransform()->position);
			if (temp < distance && HasLOS())
			{
				curNode = node;
				distance = temp;
			}
		}
	}

	if (curNode != nullptr)
	{
		SetTargetPosition(curNode->GetTransform()->position);
	}
	m_move();
}

void RangedCombatEnemy::MoveToCover()
{
	if (GetActionState() != ActionState::MOVE_TO_COVER) {
		// Initialize
		SetActionState(ActionState::MOVE_TO_COVER);
	}
	// TODO: setup another action to take when moving to the player.
}

void RangedCombatEnemy::WaitBehindCover()
{
	if (GetActionState() != ActionState::WAIT_BEHIND_COVER) {
		// Initialize
		SetActionState(ActionState::WAIT_BEHIND_COVER);
	}
	// TODO: setup another action to take when moving to the player.
}

void RangedCombatEnemy::Attack()
{
	auto scene = dynamic_cast<PlayScene*>(m_pScene);

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
		scene->SpawnEnemyTorpedo();
	}
}

void RangedCombatEnemy::m_move()
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
}

void RangedCombatEnemy::m_buildPatrolPath()
{
	m_patrolPath.push_back(glm::vec2(760, 40)); // Top Right Corner node

	m_patrolPath.push_back(glm::vec2(760, 560)); // Bottom Right Corner node

	m_patrolPath.push_back(glm::vec2(40, 560)); // Bottom Left Corner node

	m_patrolPath.push_back(glm::vec2(40, 40)); // Top Left Corner node

	SetTargetPosition(m_patrolPath[m_wayPoint]);

}

void RangedCombatEnemy::m_buildTree()
{
	// Create and add the root node - Health Condition
	m_tree->SetEnemyHealthNode(new EnemyHealthCondition(this, true));

	m_tree->GetTree().push_back(m_tree->GetEnemyHealthNode());

	m_buildLeftTree();
	m_buildRightTree();
}

void RangedCombatEnemy::m_buildLeftTree()
{
	// Left Subtree Level 1 -> Flee Action
	TreeNode* fleeNode = m_tree->AddNode(m_tree->GetEnemyHealthNode(), new FleeAction(this), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetTree().push_back(fleeNode);
}

void RangedCombatEnemy::m_buildRightTree()
{
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

	// Left Right Subtree Level 4 -> Ranged Combat Condition
	m_tree->SetRangedCombatNode(new RangedCombatCondition(this));
	m_tree->AddNode(LOSNodeLeft, m_tree->GetRangedCombatNode(), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetTree().push_back(m_tree->GetRangedCombatNode());

	// Left Left Subtree Level 5 -> Move to Range Action
	TreeNode* moveToRangeNode = m_tree->AddNode(m_tree->GetRangedCombatNode(), new MoveToRangeAction(this), TreeNodeType::LEFT_TREE_NODE);
	//dynamic_cast<ActionNode*>(moveToRangeNode)->SetAgent(this);
	m_tree->GetTree().push_back(moveToRangeNode);

	// Left Right Subtree Level 5 -> Attack Action
	TreeNode* attackNode = m_tree->AddNode(m_tree->GetRangedCombatNode(), new AttackAction(this), TreeNodeType::RIGHT_TREE_NODE);
	//dynamic_cast<ActionNode*>(attackNode)->SetAgent(this);
	m_tree->GetTree().push_back(attackNode);
}
