#include "AttackState.h"
#include "Enemy.h"
#include "ChaseState.h"
#include "SearchState.h"

void AttackState::Update(Enemy* enemy)
{
    enemy->Attack();

    if (!enemy->CheckCanSeePlayer())
    {
        enemy->ChangeState(new SearchState());
        return;
    }

    if (!enemy->CheckAttackRange())
    {
        enemy->ChangeState(new ChaseState());
    }
}
