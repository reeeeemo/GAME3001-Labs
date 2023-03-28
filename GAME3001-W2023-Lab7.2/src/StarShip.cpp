#include "StarShip.h"

#include "Game.h"
#include "TextureManager.h"
#include "Util.h"

StarShip::StarShip() : m_maxSpeed(20.0f),
m_turnRate(5.0f), m_accelerationRate(2.0f), m_startPosition(glm::vec2(300.0f, 500.0f))
{
	TextureManager::Instance().Load("../Assets/textures/d7_small.png", "starship");

	const auto size = TextureManager::Instance().GetTextureSize("starship");
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
}

StarShip::~StarShip()
= default;

void StarShip::Draw()
{
	// draw the StarShip
	TextureManager::Instance().Draw("starship", 
		GetTransform()->position, static_cast<double>(GetCurrentHeading()), 255, true);

	// draw the LOS Line
	Util::DrawLine(GetTransform()->position,
		GetTransform()->position + GetCurrentDirection() * GetLOSDistance(), GetLOSColour());
}

void StarShip::Update()
{
	// Determine which action to perform
	switch(GetActionState())
	{
	case ActionState::PATROL:
		m_move();
		break;
	case ActionState::MOVE_TO_LOS:
		break;
	case ActionState::MOVE_TO_PLAYER:
		break;
	case ActionState::ATTACK:
		break;
	}
}

void StarShip::Clean()
{
}

float StarShip::GetMaxSpeed() const
{
	return m_maxSpeed;
}

float StarShip::GetTurnRate() const
{
	return m_turnRate;
}

float StarShip::GetAccelerationRate() const
{
	return m_accelerationRate;
}

glm::vec2 StarShip::GetDesiredVelocity() const
{
	return m_desiredVelocity;
}

void StarShip::SetMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void StarShip::SetTurnRate(const float angle)
{
	m_turnRate = angle;
}

void StarShip::SetAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void StarShip::SetDesiredVelocity(const glm::vec2 target_position)
{
	//SetTargetPosition(target_position);
	m_desiredVelocity = Util::Normalize(target_position - GetTransform()->position);
	//GetRigidBody()->velocity = m_desiredVelocity - GetRigidBody()->velocity;
}

void StarShip::Seek()
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

void StarShip::LookWhereYoureGoing(const glm::vec2 target_direction)
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

void StarShip::Reset()
{
	GetTransform()->position = m_startPosition;
}

void StarShip::m_move()
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

void StarShip::m_buildPatrolPath()
{
	m_patrolPath.push_back(glm::vec2(760, 40)); // Top Right Corner node

	m_patrolPath.push_back(glm::vec2(760, 560)); // Bottom Right Corner node

	m_patrolPath.push_back(glm::vec2(40, 560)); // Bottom Left Corner node

	m_patrolPath.push_back(glm::vec2(40, 40)); // Top Left Corner node

	SetTargetPosition(m_patrolPath[m_wayPoint]);

}
