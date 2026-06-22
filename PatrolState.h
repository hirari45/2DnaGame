#pragma once
#include "EnemyStateBase.h"
class PatrolState :
    public EnemyStateBase
{
public:
    void Enter(Enemy* enemy) override;
    void Update(Enemy* enemy) override;
    void Exit(Enemy* enemy) override;
};

