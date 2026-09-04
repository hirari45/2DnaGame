#include "Enemy.h"
#include "time.h"
#include "Stage.h"
#include "Player.h"
#include "SearchState.h"
#include "PatrolState.h"
#include "ChaseState.h"
#include <math.h>

namespace
{
	const int ENEMY_SIZE = 48; //敵のサイズ 32*32
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //敵の初期位置
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32; //敵の描画サイズ
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;
}


Enemy::Enemy()
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	pos_ = ENEMY_START_POS; //32はブロックの位置pos_
	dir_ = INIT_ENEMY_DIR;

	state_ = new PatrolState();
	state_->Enter(this);
}

Enemy::~Enemy()
{
	delete state_;
}

void Enemy::Update()
{
	if (state_)
	{
		state_->Update(this);
	}

	ApplyStateChange();
}

void Enemy::Draw()
{
	DrawVisionCone();

	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;
	int nowFrame = animFrame[frame];

	Rect iRect[4] =
	{
		{ nowFrame * ENEMY_SIZE, 3 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{ nowFrame * ENEMY_SIZE, 0 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{ nowFrame * ENEMY_SIZE, 1 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{ nowFrame * ENEMY_SIZE, 2 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE}
	};

	DrawBox(pos_.x, pos_.y,
		pos_.x + ENEMY_DRAW_SIZE,
		pos_.y + ENEMY_DRAW_SIZE,
		GetColor(255, 255, 0),
		FALSE,
		2);

	DrawRectExtendGraph(
		pos_.x,
		pos_.y,
		pos_.x + ENEMY_DRAW_SIZE,
		pos_.y + ENEMY_DRAW_SIZE,
		iRect[dir_].x,
		iRect[dir_].y,
		iRect[dir_].w,
		iRect[dir_].h,
		hImage_,
		TRUE);

	if (animTimer < 0)
	{
		frame = (++frame) % 4;
		animTimer += ANIM_INTERVAL;
	}

	animTimer -= Time::DeltaTime();
}

void Enemy::ChangeState(EnemyStateBase* nextState)
{
	nextState_ = nextState;
}

void Enemy::ApplyStateChange()
{
	if (nextState_ == nullptr)
	{
		return;
	}

	state_->Exit(this);
	delete state_;

	state_ = nextState_;
	nextState_ = nullptr;

	state_->Enter(this);
}

bool Enemy::CheckCanSeePlayer()
{
	Player* player = FindGameObject<Player>();

	Point pPos = player->GetPlayerPos();

	int enemyX = pos_.x / CHA_SIZE;
	int enemyY = pos_.y / CHA_SIZE;

	int playerX = pPos.x / CHA_SIZE;
	int playerY = pPos.y / CHA_SIZE;

	int dx = playerX - enemyX;
	int dy = playerY - enemyY;

	int distance = abs(dx) + abs(dy);

	if (distance > 5)
	{
		return false;
	}

	// 横方向チェック
	if (enemyY == playerY)
	{
		int start = min(enemyX, playerX);
		int end = max(enemyX, playerX);

		for (int x = start + 1; x < end; x++)
		{
			if (FindGameObject<Stage>()->GetMap(x, enemyY) == 1)
			{
				return false;
			}
		}

		return true;
	}

	// 縦方向チェック
	if (enemyX == playerX)
	{
		int start = min(enemyY, playerY);
		int end = max(enemyY, playerY);

		for (int y = start + 1; y < end; y++)
		{
			if (FindGameObject<Stage>()->GetMap(enemyX, y) == 1)
			{
				return false;
			}
		}

		return true;
	}

	return false;

}

bool Enemy::CheckAttackRange()
{
	Player* player = FindGameObject<Player>();

	Point pPos = player->GetPlayerPos();

	int dx = abs(pPos.x - pos_.x);
	int dy = abs(pPos.y - pos_.y);

	return (dx + dy) <= CHA_SIZE;
}

bool Enemy::CheckSearchTimeOver()
{
	return searchTimer_ <= 0.0f;
}

void Enemy::Patrol()
{
	static float dir_timer = 3.0f;
	static float prog_timer = 0.25f;

	float dt = Time::DeltaTime();

	dir_timer -= dt;
	prog_timer -= dt;

	Point newPos = pos_;

	if (prog_timer < 0.0f)
	{
		switch (dir_)
		{
		case UP:    newPos.y -= ENEMY_DRAW_SIZE; break;
		case DOWN:  newPos.y += ENEMY_DRAW_SIZE; break;
		case LEFT:  newPos.x -= ENEMY_DRAW_SIZE; break;
		case RIGHT: newPos.x += ENEMY_DRAW_SIZE; break;
		}

		int mapValue =
			FindGameObject<Stage>()
			->GetMap(newPos.x / CHA_SIZE, newPos.y / CHA_SIZE);

		if (mapValue != 1)
		{
			pos_ = newPos;
		}
		else
		{
			dir_ = (DIR)GetRand(3);
		}

		prog_timer = 0.5f;
	}
}

void Enemy::Chase()
{
	Player* player = FindGameObject<Player>();

	Point pPos = player->GetPlayerPos();

	int dx = pPos.x - pos_.x;
	int dy = pPos.y - pos_.y;

	if (abs(dx) > abs(dy))
	{
		dir_ = (dx > 0) ? RIGHT : LEFT;
	}
	else
	{
		dir_ = (dy > 0) ? DOWN : UP;
	}
	static float moveTimer = 0.25f;

	moveTimer -= Time::DeltaTime();

	if (moveTimer > 0.0f)
		return;

	moveTimer = 0.25f;

	Point newPos = pos_;

	switch (dir_)
	{
	case UP:    newPos.y -= CHA_SIZE; break;
	case DOWN:  newPos.y += CHA_SIZE; break;
	case LEFT:  newPos.x -= CHA_SIZE; break;
	case RIGHT: newPos.x += CHA_SIZE; break;
	}

	if (FindGameObject<Stage>()->GetMap(
		newPos.x / CHA_SIZE,
		newPos.y / CHA_SIZE) != 1)
	{
		pos_ = newPos;
	}
}

void Enemy::Attack()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);

	int enemyX = pos_.x / CHA_SIZE;
	int enemyY = pos_.y / CHA_SIZE;

	switch (dir_)
	{
	case RIGHT:
		DrawBox(
			(enemyX + 1) * CHA_SIZE,
			enemyY * CHA_SIZE,
			(enemyX + 2) * CHA_SIZE,
			(enemyY + 1) * CHA_SIZE,
			GetColor(255, 0, 0),
			TRUE);
		break;

	case LEFT:
		DrawBox(
			(enemyX - 1) * CHA_SIZE,
			enemyY * CHA_SIZE,
			enemyX * CHA_SIZE,
			(enemyY + 1) * CHA_SIZE,
			GetColor(255, 0, 0),
			TRUE);
		break;

	case UP:
		DrawBox(
			enemyX * CHA_SIZE,
			(enemyY - 1) * CHA_SIZE,
			(enemyX + 1) * CHA_SIZE,
			enemyY * CHA_SIZE,
			GetColor(255, 0, 0),
			TRUE);
		break;

	case DOWN:
		DrawBox(
			enemyX * CHA_SIZE,
			(enemyY + 1) * CHA_SIZE,
			(enemyX + 1) * CHA_SIZE,
			(enemyY + 2) * CHA_SIZE,
			GetColor(255, 0, 0),
			TRUE);
		break;
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void Enemy::Search()
{
	searchTimer_ -= Time::DeltaTime();
}

void Enemy::DrawVisionCone()
{
	const float VIEW_ANGLE = 90.0f;
	const int VIEW_DISTANCE = 3;

	int enemyX = pos_.x / CHA_SIZE;
	int enemyY = pos_.y / CHA_SIZE;

	float dirAngle = 0.0f;

	switch (dir_)
	{
	case RIGHT: dirAngle = 0.0f; break;
	case DOWN:  dirAngle = 90.0f; break;
	case LEFT:  dirAngle = 180.0f; break;
	case UP:    dirAngle = 270.0f; break;
	}

	float halfAngle = VIEW_ANGLE / 2.0f;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);

	// 周囲3マスを調べる
	for (int y = -VIEW_DISTANCE; y <= VIEW_DISTANCE; y++)
	{
		for (int x = -VIEW_DISTANCE; x <= VIEW_DISTANCE; x++)
		{
			int cellX = enemyX + x;
			int cellY = enemyY + y;

			// 自分のマスは描画しない
			if (x == 0 && y == 0)
				continue;

			// 距離
			float distance = sqrtf((float)(x * x + y * y));

			if (distance > VIEW_DISTANCE)
				continue;

			// 角度
			float angle = atan2f((float)y, (float)x) * 180.0f / DX_PI_F;

			if (angle < 0)
				angle += 360.0f;

			float diff = fabs(angle - dirAngle);

			if (diff > 180)
				diff = 360 - diff;

			if (diff <= halfAngle)
			{
				DrawBox(
					cellX * CHA_SIZE,
					cellY * CHA_SIZE,
					(cellX + 1) * CHA_SIZE,
					(cellY + 1) * CHA_SIZE,
					GetColor(255, 255, 0),
					TRUE);
			}
		}
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

