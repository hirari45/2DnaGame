#pragma once
#include ".\Library\GameObject.h"

class Enemy;
class EnemyStateBase 
{
public:
    virtual ~EnemyStateBase() {}

    virtual void Enter(Enemy* enemy) {}
    virtual void Update(Enemy* enemy) = 0;
    virtual void Exit(Enemy* enemy) {}
};

