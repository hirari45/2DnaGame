#pragma once
#include "EnemyStateBase.h"
class ChaseState :
    public EnemyStateBase
{
public:
    void Update(Enemy* enemy) override;
};

