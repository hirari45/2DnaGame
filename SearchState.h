#pragma once
#include "EnemyStateBase.h"
class SearchState :
    public EnemyStateBase
{
public:
    void Enter(Enemy* enemy) override;
    void Update(Enemy* enemy) override;
};

