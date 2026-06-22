#include "PatrolState.h"
#include "Enemy.h"
#include "ChaseState.h"

void PatrolState::Enter(Enemy* enemy)
{
}

void PatrolState::Update(Enemy* enemy)
{
    enemy->Patrol();

    if (enemy->CheckCanSeePlayer())
    {
        enemy->ChangeState(new ChaseState());
    }
}

void PatrolState::Exit(Enemy* enemy)
{
}
