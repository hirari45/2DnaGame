#pragma once
#include ".\Library\GameObject.h"
#include "global.h"

class EnemyStateBase;

class Enemy : public GameObject
{
    int hImage_;
    Point pos_;
    DIR dir_;
    float searchTimer_ = 0.0f;

private:
    EnemyStateBase* state_ = nullptr;
    EnemyStateBase* nextState_ = nullptr;

public:
    Enemy();
    ~Enemy();

    void Update();
    void Draw();

    void ChangeState(EnemyStateBase* nextState);
    void ApplyStateChange();

    bool CheckCanSeePlayer();
    bool CheckAttackRange();
    bool CheckSearchTimeOver();

    void Patrol();
    void Chase();
    void Attack();
    void Search();

    void DrawVisionCone();
};

