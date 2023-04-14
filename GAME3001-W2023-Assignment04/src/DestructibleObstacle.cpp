#include "DestructibleObstacle.h"

#include <iostream>

#include "TextureManager.h"
#include "Util.h"

DestructibleObstacle::DestructibleObstacle()
{
    TextureManager::Instance().Load("../Assets/textures/cloud.png", "destruct_obstacle");

    const auto size = TextureManager::Instance().GetTextureSize("destruct_obstacle");
    SetWidth(static_cast<int>(size.x));
    SetHeight(static_cast<int>(size.y));

    GetTransform()->position = glm::vec2(400.0f, 300.0f);
    //GetRigidBody()->bounds = glm::vec2(GetWidth(), GetHeight());
    //setIsCentered(true);
    SetType(GameObjectType::DESTRUCT_OBSTACLE);
    GetRigidBody()->isColliding = false;
}

void DestructibleObstacle::Draw()
{
    TextureManager::Instance().Draw("destruct_obstacle", GetTransform()->position, this, 0, 255, true);
    Util::DrawFilledRect(GetTransform()->position - glm::vec2((GetHealth() / GetMaxHealth() * 100) / 2, 60.0f), GetHealth() / GetMaxHealth() * 100, 10.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

void DestructibleObstacle::Update()
{
    if(GetHealth()<=0)
    {
        Clean();
    }
}

void DestructibleObstacle::Clean()
{
    
}

void DestructibleObstacle::SetHealth(float health)
{
    m_health = health;
}

float DestructibleObstacle::GetHealth()
{
    return m_health;
}

void DestructibleObstacle::SetMaxHealth(float health)
{
    m_maxHealth = health;
}

float DestructibleObstacle::GetMaxHealth()
{
    return m_maxHealth;
}

void DestructibleObstacle::TakeDamage(float dmg)
{
    std::cout << "Obstacle took: " << dmg << " damage.\n";
    m_health-=dmg;
}
