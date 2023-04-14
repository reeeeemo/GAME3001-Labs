#pragma once
#ifndef __ENEMY__
#define __ENEMY__
#include "Agent.h"
#include "EnemyAnimationState.h"	

class Enemy : public Agent
{
public:
	Enemy();
	virtual ~Enemy();

	// Life Cycle Methods
	void Draw() override = 0;
	void Update() override = 0;
	void Clean() override = 0;

	
	void SetHealth (float health);
	void SetSpeed (float speed);
	void SetMaxSpeed (float maxSpeed);
	void SetScoreValue (float scoreValue);
	void SetDeleteMe(bool deleteMe);
	void SetTargetPlayer(GameObject* targetPlayer);
	void SetAttackDamage(float damage);
	void SetDetectionRadius(float radius);
	virtual void TakeDamage(float damage);
	void SetDetectingPlayer(bool value);
	void SetAnimationState(EnemyAnimationState state);

	[[nodiscard]] float GetSpeed() const;
	[[nodiscard]] float GetMaxSpeed() const;
	[[nodiscard]] float GetHealth() const;
	[[nodiscard]] float GetScoreValue() const;
	[[nodiscard]] bool GetDeleteMe() const;
	[[nodiscard]] GameObject* GetTargetPlayer() const;
	[[nodiscard]] float GetAttackDamage() const; 
	[[nodiscard]] float GetDetectionRadius() const;
	[[nodiscard]] float GetMaxHealth() const;
	[[nodiscard]] bool IsDetectingPlayer();
	[[nodiscard]] EnemyAnimationState GetAnimationState() const;

	//void SetAnimationState()
private:
	float m_Health;
	float m_maxHealth;
	float m_Speed;
	float m_maxSpeed;
	bool m_deleteMe;
	float m_scorevalue;
	float m_detectionRadius;
	glm::vec2 m_veloDamp;
	GameObject* m_pTargetPlayer;
	float m_attackDamage;
	bool m_detectedPlayer;
	EnemyAnimationState m_anim_state;
};

// Holds all the enemies in the scene and deals with the deletion
class EnemyPool : public DisplayObject
{
public:
	EnemyPool();

	// Lifecycle functions
	void Update() override;
	void Clean() override;
	void Draw() override;

	// Specific functions
	std::vector<Enemy*> GetPool();
	void Spawn(Enemy* enemyType);
	void UpdateTargetPlayer(GameObject* targetObject) const;

private:
	std::vector<Enemy*> m_pEnemies;
}; 



#endif /* defined (__ENEMY__) */