#include "Player.h"

#include "EventManager.h"
#include "Game.h"
#include "TextureManager.h"
#include "Util.h"

Player::Player(): m_currentAnimationState(PlayerAnimationState::PLAYER_IDLE_RIGHT)
{
	TextureManager::Instance().LoadSpriteSheet(
		"../Assets/sprites/Player/bunny_body.txt",
		"../Assets/sprites/Player/bunny_body.png", 
		"bunny_body");

	SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("bunny_body"));

	SetRangeOfAttack(50.0f);

	// set frame width
	SetWidth(64);

	// set frame height
	SetHeight(64);

	GetTransform()->position = glm::vec2(400.0f, 300.0f);
	GetRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->velocityDampening = glm::vec2(0.90f,0.90f);
	GetRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->isColliding = false;
	SetType(GameObjectType::PLAYER);
	SetHealth(100.0f);

	m_damage = 10.0f;

	BuildAnimations();
}

Player::~Player()
= default;

void Player::Draw()
{
	// If we are in debug mode, draw the collider rect.
	if(Game::Instance().GetDebugMode())
	{
		Util::DrawRect(GetTransform()->position -
				glm::vec2(this->GetWidth() * 0.5f, this->GetHeight() * 0.5f),
				this->GetWidth(), this->GetHeight());

		Util::DrawCircle(GetTransform()->position, GetRangeOfAttack());

	}

	// Drawing the health bar for the player based on the health count.
	Util::DrawFilledRect(GetTransform()->position - glm::vec2((m_Health / m_maxHealth * 100) / 2, 60.0f), m_Health / m_maxHealth * 100, 10.0f, glm::vec4(0, 1.0f, 0, 1.0f));

	// draw the player according to animation state
	switch(m_currentAnimationState)
	{
	case PlayerAnimationState::PLAYER_IDLE_RIGHT:
		TextureManager::Instance().PlayAnimation("bunny_body", GetAnimation("idle"),
			GetTransform()->position, 0.12f, GetTransform()->rotation.r, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case PlayerAnimationState::PLAYER_IDLE_LEFT:
		TextureManager::Instance().PlayAnimation("bunny_body", GetAnimation("idle"),
			GetTransform()->position, 0.12f,GetTransform()->rotation.r, 255, true);
		break;
	case PlayerAnimationState::PLAYER_RUN_RIGHT:
		TextureManager::Instance().PlayAnimation("bunny_body", GetAnimation("run"),
			GetTransform()->position, 0.15f, GetTransform()->rotation.r, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case PlayerAnimationState::PLAYER_RUN_LEFT:
		TextureManager::Instance().PlayAnimation("bunny_body", GetAnimation("run"),
			GetTransform()->position, 0.15f, GetTransform()->rotation.r, 255, true);
		break;
	case PlayerAnimationState::PLAYER_RUN_DOWN:
		TextureManager::Instance().PlayAnimation("bunny_body", GetAnimation("run_back"),
			GetTransform()->position, 0.15f, GetTransform()->rotation.r, 255, true);
		break;
	case PlayerAnimationState::PLAYER_RUN_UP:
		TextureManager::Instance().PlayAnimation("bunny_body", GetAnimation("run_front"),
			GetTransform()->position, 0.15f, GetTransform()->rotation.r, 255, true);
		break;
	default:
		break;
	}
}

void Player::Update()
{
	Move();
	m_LookAtMouse();
	if (Util::Magnitude(GetRigidBody()->velocity) <= 10)
	{
		if (m_currentAnimationState != PlayerAnimationState::PLAYER_IDLE_LEFT && m_currentAnimationState != PlayerAnimationState::PLAYER_IDLE_RIGHT)
		{
			if (m_currentAnimationState == PlayerAnimationState::PLAYER_RUN_LEFT)
			{
				SetAnimationState(PlayerAnimationState::PLAYER_IDLE_LEFT);
			}
			else
			{
				SetAnimationState(PlayerAnimationState::PLAYER_IDLE_RIGHT);
			}
		}
		
	}
}

void Player::Clean()
{
}

void Player::Move()
{
	if (GetRigidBody()->isColliding)
	{
		GetRigidBody()->velocity = -GetRigidBody()->velocity*2.0f;
	}
	const float dt =Game::Instance().GetDeltaTime();
	const glm::vec2 initial_position = GetTransform()->position;
	const glm::vec2 velocity_term = GetRigidBody()->velocity * dt;
	const glm::vec2 acceleration_term = GetRigidBody()->acceleration * 0.5f;
	const glm::vec2 final_position = initial_position + velocity_term + acceleration_term;
	GetTransform()->position = final_position;
	GetRigidBody()->velocity += GetRigidBody()->acceleration;
	GetRigidBody()->velocity*=GetRigidBody()->velocityDampening;
}

void Player::MeleeAttack()
{
	// Melee Animation here

	std::cout << "Melee Attack successful on enemy!\n\n";
}


void Player::SetAnimationState(const PlayerAnimationState new_state)
{
	m_currentAnimationState = new_state;
}

void Player::SetHealth(float health)
{
	m_Health = health;
	m_maxHealth = m_Health;
}


float Player::GetHealth() const
{
	return m_Health;
}

float Player::GetRangeOfAttack()
{
	return m_rangeOfAttack;
}

float Player::GetDamage()
{
	return m_damage;
}

void Player::TakeDamage(float dmg)
{
	m_Health-= dmg;
	std::cout << "Player took " << dmg << " damage.\n";
}

void Player::SetRangeOfAttack(float value)
{
	m_rangeOfAttack = value;
}

void Player::BuildAnimations()
{
	auto idle_animation = Animation();

	idle_animation.name = "idle";
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("idle1"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("idle2"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("idle3"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("idle4"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("idle5"));


	SetAnimation(idle_animation);

	auto run_back_animation = Animation();

	run_back_animation.name = "run_back";
	run_back_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_back1"));
	run_back_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_back2"));
	run_back_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_back3"));

	SetAnimation(run_back_animation);

	auto run_animation = Animation();

	run_animation.name = "run";
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("run1"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("run2"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("run3"));

	SetAnimation(run_animation);

	auto run_front_animation = Animation();

	run_front_animation.name = "run_front";
	run_front_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_front1"));
	run_front_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_front2"));
	run_front_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_front3"));

	SetAnimation(run_front_animation);
}

void Player::m_LookAtMouse()
{
	glm::vec2 mousePos = EventManager::Instance().GetMousePosition();
	float angleToMouse = atan2(mousePos.y-GetTransform()->position.y,mousePos.x-GetTransform()->position.x);
	GetTransform()->rotation.r = angleToMouse*Util::Rad2Deg;
	while (GetTransform()->rotation.r < 0) GetTransform()->rotation.r += 360;
	while (GetTransform()->rotation.r > 360) GetTransform()->rotation.r -= 360;
	if (GetTransform()->rotation.r > 90 && GetTransform()->rotation.r < 270)
	{
		m_flip = SDL_FLIP_NONE;
	}
	else
	{
		m_flip = SDL_FLIP_HORIZONTAL;

	}
}

