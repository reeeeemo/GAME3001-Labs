#pragma once
#include "Obstacle.h"

class DestructibleObstacle final : public Obstacle
{
public:
    DestructibleObstacle();
    void Draw() override;
    void Update() override;
    void Clean() override;
    
    void SetHealth(float health);
    float GetHealth();

    void SetMaxHealth(float health);
    float GetMaxHealth();

    void TakeDamage(float dmg);
    
private:
    float m_health;
    float m_maxHealth;
    
};
