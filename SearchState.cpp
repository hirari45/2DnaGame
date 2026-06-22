#include "SearchState.h"
#include "Enemy.h"
#include "PatrolState.h"
#include "ChaseState.h"

void SearchState::Enter(Enemy* enemy)
{
}

void SearchState::Update(Enemy* enemy)
{
    enemy->Search();

    if (enemy->CheckCanSeePlayer())
    {
        enemy->ChangeState(new ChaseState());
        return;
    }

    if (enemy->CheckSearchTimeOver())
    {
        enemy->ChangeState(new PatrolState());
    }
}
