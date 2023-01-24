#include "Starship.h"
#include "TextureManager.h"
#include "Util.h"
#include "Game.h"

Starship::Starship() {
	TextureManager::Instance().Load("../Assets/textures/ncl.png", "starship");

	const auto size = TextureManager::Instance().GetTextureSize("");
	SetWidth(static_cast<int>(size.x));
	SetHeight(static_cast<int>(size.y));
	GetTransform()->position = glm::vec2(0.0f,0.0f);
	GetRigidBody()->bounds = glm::vec2(GetWidth(), GetHeight());
	GetRigidBody()->velocity = glm::vec2(0, 0);
	GetRigidBody()->acceleration = glm::vec2(0,0);
	GetRigidBody()->isColliding = false;

	// Starting motion properties
	m_maxSpeed = 100.0f; // A maximum number of pixels moved per frame
	m_turnRate = 5.0f; // A maximum number of degrees to turn each time-step
	m_accelerationRate = 4.0f; // A maximum number of pixels to add to the velocity each frame

	SetType(GameObjectType::AGENT);
}

Starship::~Starship()
= default;

void Starship::Draw() {
	TextureManager::Instance().Draw("starship", GetTransform()->position, GetCurrentHeading(), 255, true);

}

void Starship::Update() {
	m_move();
}

void Starship::Clean() {

}

float Starship::GetMaxSpeed() const
{
	return m_maxSpeed;
}

float Starship::GetTurnRate()
{
	return m_turnRate;
}

float Starship::GetAccelerationRate() const
{
	return m_accelerationRate;
}

glm::vec2 Starship::GetDesiredVelocity() const
{
	return m_desiredVelocity;
}

void Starship::SetMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void Starship::SetTurnRate(const float angle)
{
	m_turnRate = angle;
}

void Starship::SetAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void Starship::SetDesiredVelocity(const glm::vec2 target_position)
{
	SetTargetPosition(target_position);
	m_desiredVelocity = Util::Normalize(target_position - GetTransform()->position);
	GetRigidBody()->velocity = m_desiredVelocity - GetRigidBody()->velocity;

}

void Starship::Seek()
{
	SetDesiredVelocity(GetTargetPosition());

	const glm::vec2 steering_direction = GetDesiredVelocity() - GetCurrentDirection();

	LookWhereYoureGoing(steering_direction);

	GetRigidBody()->acceleration = GetCurrentDirection() * GetAccelerationRate();

}

void Starship::LookWhereYoureGoing(glm::vec2 target_direction)
{
	const float target_rotation = Util::SignedAngle(GetCurrentDirection(), target_direction);

	const float turn_sensitivity = 5.0f;

	if (abs(target_rotation) > turn_sensitivity)
	{
		if (target_rotation > 0.0f)
		{
			SetCurrentHeading(GetCurrentHeading() + GetTurnRate());
		}
		else if (target_rotation < 0.0f)
		{
			SetCurrentHeading(GetCurrentHeading() - GetTurnRate());
		}
	}
}

void Starship::m_move()
{
	Seek();
	//                       Final Pos  Pos Term       Velocity Term              Acceleration Term
	// kinematic equation -> Pf =       Pi +           Vi * (time) +              (0.5) * Ai * (tine * time)
	const float dt = Game::Instance().GetDeltaTime();

	// Compute the position Term
	const glm::vec2 initial_position = GetTransform()->position;

	// Compute the velocity Term
	const glm::vec2 velocity_term = GetRigidBody()->velocity * dt; // 

	// Compute the acceleration Term
	const glm::vec2 acceleration_term = GetRigidBody()->acceleration * 0.5f; // Usually multiply by dt, but it will be too slow


	// Compute the new position
	glm::vec2 final_position = initial_position + velocity_term + acceleration_term;

	GetTransform()->position = final_position;

	// Add our acceleration to the velocity
	GetRigidBody()->velocity += GetRigidBody()->acceleration;

	// Clamp our velocity at max speed
	GetRigidBody()->velocity = Util::Clamp(GetRigidBody()->velocity, GetMaxSpeed());
}

