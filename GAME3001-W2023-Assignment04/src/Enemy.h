#pragma once
#include "Agent.h"
#include "DecisionTree.h"
#include "Sprite.h"

class Enemy : public Agent
{
public:
    void Draw() override = 0;
    void Update() override = 0;
    void Clean() override = 0;
    [[nodiscard]] float GetMaxSpeed() const;
    [[nodiscard]] float GetTurnRate() const;
    [[nodiscard]] float GetAccelerationRate() const;
    [[nodiscard]] glm::vec2 GetDesiredVelocity() const;
    [[nodiscard]] float GetMaxRange() const;
    [[nodiscard]] float GetMinRange() const;
    [[nodiscard]] DecisionTree* GetTree() const;
    [[nodiscard]] Scene* GetScene() const;
    [[nodiscard]] float GetLastRotation() const;
    [[nodiscard]] glm::vec2 GetStartPos() const;
    [[nodiscard]] Sprite* GetSprite() const;
    [[nodiscard]] std::vector<glm::vec2> GetPatrolPath() const;
    [[nodiscard]] int GetWaypoint() const;
    [[nodiscard]] bool GetMovingTowardsPlayer() const;

    void SetMaxSpeed(float speed);
    void SetTurnRate(float angle);
    void SetAccelerationRate(float rate);
    void SetDesiredVelocity(glm::vec2 target_position);
    void SetMaxRange(float range);
    void SetMinRange(float range);
    void SetTree(DecisionTree* tree);
    void SetScene(Scene* scene);
    void SetLastRotation(float rotate);
    void SetStartPos(glm::vec2 pos);
    void SetSprite(Sprite* sprite);
    void SetPatrolPath(std::vector<glm::vec2> path);
    void SetWaypoint(int waypoint);
    void SetMovingTowardsPlayer(bool towards);

    // public functions
    virtual void Seek();
    virtual void LookWhereYoureGoing(glm::vec2 target_direction, bool direction_hack = true);
    virtual void Reset();
    virtual void buildPatrolPath();

    // New for Lab 7.2
    void Patrol() override;
    void MoveToLOS() override;
    void MoveToPlayer() override;
    void Attack() override = 0;
    void MoveToRange() override;


    // New for Lab 7.3
    void Flee() override;
    void MoveToCover() override;
    void WaitBehindCover() override;

private:
    virtual void BuildAnimations() = 0;
    virtual void m_buildTree() = 0;
    virtual void m_move();
    // private movement variables
    float m_maxSpeed;
    float m_turnRate;
    float m_lastRotation;
    float m_accelerationRate;
    glm::vec2 m_startPosition;
    Sprite* m_sprite;
    glm::vec2 m_desiredVelocity;
    std::vector<glm::vec2> m_patrolPath;
    int m_wayPoint;
    DecisionTree* m_tree;
    Scene* m_pScene;
    bool m_movingTowardsPlayer;
    bool m_behindCover;
    bool m_isFleeing;
    float coverTimer;

    float m_maxRangeDistance;
    float m_minRangeDistance;
    
};
