#include "StarShip.h"

#include "Game.h"
#include "TextureManager.h"
#include "Util.h"
#include "Sprite.h"

StarShip::StarShip()
{
	//TextureManager::Instance().Load("../Assets/textures/cow.png", "cow");
	TextureManager::Instance().LoadSpriteSheet("../Assets/sprites/cow.txt", "../Assets/sprites/cow.png", "cow");
	SetWidth(64);
	SetHeight(64);

	SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("cow"));
	//const auto size = TextureManager::Instance().GetTextureSize("cow");
	//SetWidth(static_cast<int>(size.x));
	//SetHeight(static_cast<int>(size.y));
	GetTransform()->position = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->bounds = glm::vec2(GetWidth(), GetHeight());
	GetRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->isColliding = false;
	setIsCentered(true);
	SetType(GameObjectType::AGENT);

	// Starting Motion Properties
	m_maxSpeed = 20.0f; // a maximum number of pixels moved per frame
	m_turnRate = 5.0f; // a maximum number of degrees to turn each time-step
	m_accelerationRate = 4.0f; // a maximum number of pixels to add to the velocity each frame

	SetCurrentDirection(glm::vec2(1.0f, 0.0f)); // Facing Right

	SetLOSDistance(300.0f);
	BuildAnimations();
}

StarShip::~StarShip()
= default;

void StarShip::Draw()
{
	// draw the target
	//TextureManager::Instance().Draw("cow", 
		//GetTransform()->position, static_cast<double>(GetCurrentHeading()), 255, true);
	// draw the player according to animation state
	switch (m_currentAnimationState)
	{
	case CowAnimationState::COW_IDLE:
		TextureManager::Instance().PlayAnimation("cow", GetAnimation("idle"),
			GetTransform()->position, 0.12f, 0, 255, true);
		break;
	case CowAnimationState::COW_RUN:
		TextureManager::Instance().PlayAnimation("cow", GetAnimation("run"),
			GetTransform()->position, 0.12f, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	default:
		break;
	}
}

void StarShip::Update()
{
	
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
		SetDesiredVelocity(GetTargetPosition());

		const glm::vec2 steering_direction = GetDesiredVelocity() - GetCurrentDirection();

		LookWhereYoureGoing(steering_direction);

		GetRigidBody()->acceleration = GetCurrentDirection() * GetAccelerationRate();
}

void StarShip::LookWhereYoureGoing(const glm::vec2 target_direction)
{
	float target_rotation = Util::SignedAngle(GetCurrentDirection(), target_direction);

	const float turn_sensitivity = 3.0f;

	SetCurrentHeading(Util::LerpUnclamped(GetCurrentHeading(), 
		GetCurrentHeading() + target_rotation, GetTurnRate() * Game::Instance().GetDeltaTime()));
}

void StarShip::m_move()
{
	Seek();
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

SpriteSheet* StarShip::GetSpriteSheet()
{
	return m_pSpriteSheet;
}

Animation& StarShip::GetAnimation(const std::string& name)
{
	return m_pAnimations[name];
}

void StarShip::SetSpriteSheet(SpriteSheet* sprite_sheet)
{
	m_pSpriteSheet = sprite_sheet;
}

void StarShip::SetAnimation(const Animation& animation)
{
	if (!CheckIfAnimationExists(animation.name))
	{
		m_pAnimations[animation.name] = animation;
	}
}

bool StarShip::CheckIfAnimationExists(const std::string& id)
{
	return m_pAnimations.find(id) != m_pAnimations.end();
}

void StarShip::BuildAnimations()
{
	auto idle_animation = Animation();

	idle_animation.name = "idle";
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("cow-idle-0"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("cow-idle-1"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("cow-idle-2"));

	SetAnimation(idle_animation);

	auto run_animation = Animation();

	run_animation.name = "run";
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("cow-move-0"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("cow-move-1"));

	SetAnimation(run_animation);
}