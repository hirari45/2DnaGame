#include "ChaseState.h"
#include "Enemy.h"
#include "PatrolState.h"
#include "AttackState.h"
#include "SearchState.h"

void ChaseState::Update(Enemy* enemy)
{
    enemy->Chase();

    if (!enemy->CheckCanSeePlayer())
    {
        enemy->ChangeState(new SearchState());
        return;
    }

    if (enemy->CheckAttackRange())
    {
        enemy->ChangeState(new AttackState());
    }
}
