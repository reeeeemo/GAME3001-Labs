#pragma once
#ifndef __TORPEDO_H__
#define __TORPEOD_H__

#include "TorpedoAnimationState.h"
#include "Sprite.h"
#include "GameObject.h"
#include <vector>

enum TORPEDO_TYPE
{
	ENEMY,
	PLAYER,
};

class Torpedo : public Sprite
{
public:
	Torpedo();
	virtual ~Torpedo() override;

	// Life Cycle Methods
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;

	// setters
	virtual void SetAnimationState(TorpedoAnimationState new_state);
	void SetDamage(float damage){m_damage=damage;}
	float GetDamage(){return m_damage;}

	bool GetDeleteMe();
	void SetDeleteMe(bool temp);

	TORPEDO_TYPE GetTorpedoType();
	void SetTorpedoType(TORPEDO_TYPE type);

protected:
	virtual void BuildAnimations();

	TorpedoAnimationState m_currentAnimationState;

	std::string m_textureKey;
	float m_speed;
	float m_damage;
	glm::vec2 m_direction;
	bool m_deleteMe;
	TORPEDO_TYPE m_type;
};

class TorpedoPool final : public DisplayObject
{
public:
	TorpedoPool();
	~TorpedoPool();

	void Draw() override;
	void Update() override;
	void Clean() override;

	void FireTorpedo(Torpedo* torpedoToFire);
	std::vector<Torpedo*> GetPool();
private:
	std::vector<Torpedo*> m_pTorpedos;
};

#endif /* defined (__TORPEDO_H__) */