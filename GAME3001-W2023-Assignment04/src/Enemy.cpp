#include "Enemy.h"

#include "Game.h"
#include "Util.h"



float Enemy::GetMaxSpeed() const
{
    return m_maxSpeed;
}

float Enemy::GetTurnRate() const
{
    return m_turnRate;
}

float Enemy::GetAccelerationRate() const
{
    return m_accelerationRate;
}

glm::vec2 Enemy::GetDesiredVelocity() const
{
    return m_desiredVelocity;
}

float Enemy::GetMaxRange() const
{
    return m_maxRangeDistance;
}

float Enemy::GetMinRange() const
{
    return m_minRangeDistance;
}


DecisionTree* Enemy::GetTree() const
{
    return m_tree;
}

Scene* Enemy::GetScene() const
{
    return m_pScene;
}

float Enemy::GetLastRotation() const
{
    return m_lastRotation;
}

glm::vec2 Enemy::GetStartPos() const
{
    return m_startPosition;
}

Sprite* Enemy::GetSprite() const
{
    return m_sprite;
}

std::vector<glm::vec2> Enemy::GetPatrolPath() const
{
    return m_patrolPath;
}

int Enemy::GetWaypoint() const
{
    return m_wayPoint;
}

bool Enemy::GetMovingTowardsPlayer() const
{
    return m_movingTowardsPlayer;
}

bool Enemy::GetIsHit() const
{
    return m_isHit;
}

void Enemy::SetIsHit(bool hit)
{
    m_isHit = hit;
}

void Enemy::SetMaxRange(float range)
{
    m_maxRangeDistance = range;
}

void Enemy::SetMinRange(float range)
{
    m_minRangeDistance = range;
}

void Enemy::SetTree(DecisionTree* tree)
{
    m_tree = tree;
}

void Enemy::SetScene(Scene* scene)
{
    m_pScene = scene;
}

void Enemy::SetLastRotation(float rotate)
{
    m_lastRotation = rotate;
}

void Enemy::SetStartPos(glm::vec2 pos)
{
    m_startPosition = pos;
}

void Enemy::SetSprite(Sprite* sprite)
{
    m_sprite = sprite;
}

void Enemy::SetPatrolPath(std::vector<glm::vec2> path)
{
    m_patrolPath = path;
}

void Enemy::SetWaypoint(int waypoint)
{
    m_wayPoint = waypoint;
}

void Enemy::SetMovingTowardsPlayer(bool towards)
{
    m_movingTowardsPlayer = towards;
}



void Enemy::SetMaxSpeed(float speed)
{
    m_maxSpeed = speed;
}

void Enemy::SetTurnRate(float angle)
{
    m_turnRate = angle;
}

void Enemy::SetAccelerationRate(float rate)
{
    m_accelerationRate = rate;
}

void Enemy::SetDesiredVelocity(glm::vec2 target_position)
{
    m_desiredVelocity = Util::Normalize(target_position - GetTransform()->position);
}

void Enemy::Seek()
{
    // Find next waypoint if within 10px of the current waypoint
    if (!m_movingTowardsPlayer) {
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
    }
	
    SetDesiredVelocity(GetTargetPosition());

    const glm::vec2 steering_direction = GetDesiredVelocity() - GetCurrentDirection();

    LookWhereYoureGoing(steering_direction, true);

    GetRigidBody()->acceleration = GetCurrentDirection() * GetAccelerationRate();
}

void Enemy::LookWhereYoureGoing(glm::vec2 target_direction, bool direction_hack)
{
    const float direction_axis = direction_hack ? 90.0f : 0.0f;
    float target_rotation = Util::SignedAngle(GetCurrentDirection(), target_direction) - direction_axis;

    m_lastRotation = target_rotation;

    if (target_rotation < 0)
    {
        target_rotation += 180.0f;
    }
    if (target_rotation < 184 && target_rotation > 176)
    {
        target_rotation = m_lastRotation;
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

void Enemy::Reset()
{
    GetTransform()->position = m_startPosition;
}

void Enemy::Patrol()
{
    if (GetActionState() != ActionState::PATROL) {
        // Initialize
        SetActionState(ActionState::PATROL);
    }
    auto tempnode = new PathNode();
    tempnode->GetTransform()->position=m_patrolPath[m_wayPoint];
    if(CheckAgentLOSToTarget(tempnode,dynamic_cast<PlayScene*>(GetScene())->GetObstacles()))
    {
        SetTargetPosition(m_patrolPath[m_wayPoint]);
        m_move();
    }
    else
    {
        MoveToLOS();
    }
    m_movingTowardsPlayer = false;
    m_isFleeing = false;
    m_behindCover = false;
}

void Enemy::MoveToLOS()
{
    auto scene = dynamic_cast<PlayScene*>(m_pScene);

    //glm::vec2 target_direction = Util::Normalize(scene->GetTarget()->GetLOSDistance() - GetTransform()->position);
   // SetTargetPosition({ scene->GetTarget()->GetTransform()->position.x, scene->GetTarget()->GetTransform()->position.y - GetLOSDistance()});
    //LookWhereYoureGoing(target_direction);
    m_movingTowardsPlayer = true;
    float distance = 1000.00f;
    PathNode* curNode = nullptr;
    for (const auto node : scene->GetGrid())
    {
        if (node->HasLOS())
        {
            float temp = Util::Distance(node->GetTransform()->position, GetTransform()->position);
            if (temp < distance && HasLOS() && distance >= GetMinRange())
            {
                curNode = node;
                distance = temp;
            }
        }
    }
    if (curNode != nullptr)
    {
        SetTargetPosition(curNode->GetTransform()->position);
    }
    m_move();
}

void Enemy::MoveToPlayer()
{
    if (!HasLOS())
    {
        MoveToLOS();
    }
    else
    {
        auto scene = dynamic_cast<PlayScene*>(m_pScene);
        m_movingTowardsPlayer = true;
        

        if (GetActionState() != ActionState::MOVE_TO_PLAYER) {
            // Initialize
            SetActionState(ActionState::MOVE_TO_PLAYER);
        }
        // TODO: setup another action to take when moving to the player.
        SetTargetPosition(scene->GetTarget()->GetTransform()->position);
        m_move();
    }
    
}

void Enemy::MoveToRange()
{
    auto scene = dynamic_cast<PlayScene*>(m_pScene);
    m_movingTowardsPlayer = true;
    PathNode* curNode = nullptr;
    float distance = 1000.00f;

    if (GetActionState() != ActionState::MOVE_TO_RANGE) {
        // Initialize
        SetActionState(ActionState::MOVE_TO_RANGE);
    }
    // TODO: setup another action to take when moving to the player.
    for (const auto node : scene->GetGrid())
    {
        float temp = Util::Distance(scene->GetTarget()->GetTransform()->position, node->GetTransform()->position);
        if (temp >= GetMinRange() && node->HasLOS() && temp < distance)
        {
            curNode = node;
            distance = temp;
        }
    }
    if (curNode != nullptr)
    {
        if(CheckAgentLOSToTarget(curNode, dynamic_cast<PlayScene*>(GetScene())->GetObstacles()))
        {
            SetTargetPosition(curNode->GetTransform()->position);
            m_move();
        }
        else
        {
            MoveToLOS();
        }
    }
    //m_move();
}


void Enemy::Flee()
{
    m_movingTowardsPlayer = true;
    if (GetActionState() != ActionState::FLEE) {
        // Initialize
        SetActionState(ActionState::FLEE);
    }
    // RUN AWAY!!!
    if (!m_isFleeing)
    {
        std::cout << "fleeing\n";
        m_isFleeing = true;
        SetTargetPosition(glm::vec2(rand() % 800 + 800, rand() % 600 + 600));
    }
    m_move();
}

void Enemy::MoveToCover()
{
    auto scene = dynamic_cast<PlayScene*>(m_pScene);
    m_movingTowardsPlayer = true;
    m_behindCover = true;

    if (GetActionState() != ActionState::MOVE_TO_COVER) {
        // Initialize
        SetActionState(ActionState::MOVE_TO_COVER);
    }
    
    float distance = 1000.00f;
    PathNode* curNode = nullptr;
    for (const auto node : scene->GetGrid())
    {
        float temp = Util::Distance(node->GetTransform()->position, GetTransform()->position);
        if (temp < distance && !node->HasLOS())
        {
            curNode = node;
            distance = temp;
        }
    }
    if (curNode != nullptr)
    {
        if(CheckAgentLOSToTarget(curNode, dynamic_cast<PlayScene*>(GetScene())->GetObstacles()))
        {
            SetTargetPosition(curNode->GetTransform()->position);
            m_move();
        }
        else
        {
            MoveToLOS();
        }
        coverTimer = rand() % 5;
    }
    
   // m_move();
}

void Enemy::WaitBehindCover()
{
    auto scene = dynamic_cast<PlayScene*>(m_pScene);
    if (GetActionState() != ActionState::WAIT_BEHIND_COVER) {
        // Initialize
        SetActionState(ActionState::WAIT_BEHIND_COVER);
    }
    // TODO: setup another action to take when moving to the player.
    if (m_behindCover)
    {
        if (coverTimer <= 0)
        {
            coverTimer = rand() % 5;
            SetIsHit(false);
        }
        coverTimer -= Game::Instance().GetDeltaTime();
    }
}

void Enemy::BuildAnimations()
{
}

void Enemy::m_move()
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

    float xDir = abs(GetCurrentDirection().x);
    float yDir = abs(GetCurrentDirection().y);

    if (!GetTree()->GetEnemyHitNode()->GetHit() && GetHealth() > 0)
    {
        if (xDir > yDir && GetCurrentDirection().x > 0)
        {
            SetAnimationState(EnemyAnimationState::ENEMY_RUN_RIGHT);
        }
        else if (xDir > yDir && GetCurrentDirection().x < 0)
        {
            SetAnimationState(EnemyAnimationState::ENEMY_RUN_LEFT);
        }

        if (yDir > xDir && GetCurrentDirection().y > 0)
        {
            SetAnimationState(EnemyAnimationState::ENEMY_RUN_UP);
        }
        else if (yDir > xDir && GetCurrentDirection().y < 0)
        {
            SetAnimationState(EnemyAnimationState::ENEMY_RUN_DOWN);
        }

        if (Util::Magnitude(GetRigidBody()->velocity) <= 5)
        {
            if (GetAnimationState() != EnemyAnimationState::ENEMY_IDLE_LEFT || GetAnimationState() != EnemyAnimationState::ENEMY_IDLE_RIGHT)
            {
                if (GetAnimationState() == EnemyAnimationState::ENEMY_RUN_LEFT)
                {
                    SetAnimationState(EnemyAnimationState::ENEMY_IDLE_LEFT);
                }
                else
                {
                    SetAnimationState(EnemyAnimationState::ENEMY_IDLE_RIGHT);
                }
            }
        }
    }
    
}

void Enemy::buildPatrolPath()
{
    m_patrolPath.push_back(glm::vec2(760, 40)); // Top Right Corner node

    m_patrolPath.push_back(glm::vec2(760, 560)); // Bottom Right Corner node

    m_patrolPath.push_back(glm::vec2(40, 560)); // Bottom Left Corner node

    m_patrolPath.push_back(glm::vec2(40, 40)); // Top Left Corner node

    SetTargetPosition(m_patrolPath[m_wayPoint]);
}


