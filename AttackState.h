#pragma once
#include "EnemyStateBase.h"
class AttackState :
    public EnemyStateBase
{
public:
    void Update(Enemy* enemy) override;
};

