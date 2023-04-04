#include "RangedCombatEnemy.h"

#include "Game.h"
#include "TextureManager.h"
#include "Util.h"
#include "MoveToLOSAction.h"
#include "MoveToPlayerAction.h"
#include "PatrolAction.h"
#include "EventManager.h"
#include "MoveToRange.h"

RangedCombatEnemy::RangedCombatEnemy() : m_maxSpeed(20.0f),
m_turnRate(5.0f), m_accelerationRate(2.0f), m_startPosition(glm::vec2(300.0f, 500.0f))
{
	TextureManager::Instance().Load("../Assets/textures/reliant_small.png", "ranged_combat_enemy");

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

void RangedCombatEnemy::LookWhereYoureGoing(const glm::vec2 target_direction)
{
	float target_rotation = Util::SignedAngle(GetCurrentDirection(), target_direction) -90.0f;

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

void RangedCombatEnemy::MoveToRange()
{
	if (GetActionState() != ActionState::MOVE_TO_RANGE) {
		// Initialize
		SetActionState(ActionState::MOVE_TO_RANGE);
	}
	// TODO: setup another action to take when moving to the player.
}

DecisionTree* RangedCombatEnemy::GetTree() const
{
	return m_tree;
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
	//Conditions

	// Create and add the root node
	m_tree->SetLOSNode(new LOSCondition(this));

	m_tree->GetTree().push_back(m_tree->GetLOSNode());


	m_tree->SetRadiusNode(new RadiusCondition(this));
	m_tree->AddNode(m_tree->GetLOSNode(), m_tree->GetRadiusNode(), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetTree().push_back(m_tree->GetRadiusNode());

	

	m_tree->SetCloseCombatNode(new CloseCombatCondition(this));
	m_tree->AddNode(m_tree->GetLOSNode(), m_tree->GetCloseCombatNode(), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetTree().push_back(m_tree->GetCloseCombatNode());

	m_tree->SetRangedCombatNode(new RangedCombatCondition(this));
	m_tree->AddNode(m_tree->GetLOSNode(), m_tree->GetRangedCombatNode(), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetTree().push_back(m_tree->GetRangedCombatNode());

	// Actions

	// Left Sub-tree
	TreeNode* patrolNode = m_tree->AddNode(m_tree->GetRadiusNode(), new PatrolAction(this), TreeNodeType::LEFT_TREE_NODE);
	//dynamic_cast<ActionNode*>(patrolNode)->SetAgent(this);
	m_tree->GetTree().push_back(patrolNode);

	TreeNode* moveToLOSNode = m_tree->AddNode(m_tree->GetRadiusNode(), new MoveToLOSAction(this), TreeNodeType::RIGHT_TREE_NODE);
	//dynamic_cast<ActionNode*>(moveToLOSNode)->SetAgent(this);
	m_tree->GetTree().push_back(moveToLOSNode);

	// Right sub-tree
	TreeNode* moveToRangeNode = m_tree->AddNode(m_tree->GetRangedCombatNode(), new MoveToRangeAction(this), TreeNodeType::LEFT_TREE_NODE);
	//dynamic_cast<ActionNode*>(moveToRangeNode)->SetAgent(this);
	m_tree->GetTree().push_back(moveToRangeNode);

	TreeNode* attackNode = m_tree->AddNode(m_tree->GetRangedCombatNode(), new AttackAction(this), TreeNodeType::RIGHT_TREE_NODE);
	//dynamic_cast<ActionNode*>(attackNode)->SetAgent(this);
	m_tree->GetTree().push_back(attackNode);
}
