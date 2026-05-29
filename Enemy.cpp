#include "Enemy.h"
#include "time.h"
#include "Stage.h"
#include "Player.h"

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
	: GameObject() 
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	pos_ = ENEMY_START_POS; //32はブロックの位置pos_
	dir_ = INIT_ENEMY_DIR;

	state_ = PATROL;
	detectRange = 200.0f; //namespace
	isChasing = false;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	//GetRand(数値)
	//3秒に1回向きをランダムに変える
	static float dir_timer = 3.0f;
	static float prog_timer = 0.25f;
	float dt = Time::DeltaTime();
	dir_timer = dir_timer - dt;
	prog_timer = prog_timer - dt;
	//if (dir_timer < 0.0f)
	//{
	//	dir_ = (DIR)(GetRand(3));
	//	dir_timer = 3.0f + dir_timer;
	//}

	CheckDistance();

	Point newPos = pos_;
	if (prog_timer < 0.0f)
	{
		switch (dir_)
		{
		case UP:
			newPos.y -= ENEMY_DRAW_SIZE;
			break;
		case DOWN:
			newPos.y += ENEMY_DRAW_SIZE;
			break;
		case LEFT:
			newPos.x -= ENEMY_DRAW_SIZE;
			break;
		case RIGHT:
			newPos.x += ENEMY_DRAW_SIZE;
			break;
		default:
			break;
		}
		int mapValue = FindGameObject<Stage>()->GetMap(newPos.x / CHA_SIZE, newPos.y / CHA_SIZE);
		//Stage* stage = FindGameObject<Stage>();
		//int mapValue = stage->GetMap(newPos.x / CHA_SIZE, newPos.y / CHA_SIZE);
		//移動先がステージの外に出ないようにする
		if (mapValue!=1)
		{
			pos_ = newPos;
		}
		else
		{
			dir_ = (DIR)(GetRand(3));
			dir_timer = 3.0f + dir_timer;

			//switch (dir_)
			//{
			//case UP:
			//	dir_=LEFT;
			//	break;
			//case DOWN:
			//	dir_ = RIGHT;
			//	break;
			//case LEFT:
			//	dir_ = DOWN;
			//	break;
			//case RIGHT:
			//	dir_ = UP;
			//	break;
			//default:
			//	break;
			//}
		}
		prog_timer = 0.5f + prog_timer;
	}

	switch (state_)
	{
	case PATROL:
		Patrol();
		break;

	case CHASE:
		Chase();
		break;
	}

}

void Enemy::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	int enemyX = pos_.x / CHA_SIZE;
	int enemyY = pos_.y / CHA_SIZE;

	// 視界サイズ
	const int viewSize = 3;

	switch (dir_)
	{
	case RIGHT:

		for (int y = -1; y <= 1; y++)
		{
			for (int x = 1; x <= 3; x++)
			{
				DrawBox(
					(enemyX + x) * CHA_SIZE,
					(enemyY + y) * CHA_SIZE,
					(enemyX + x + 1) * CHA_SIZE,
					(enemyY + y + 1) * CHA_SIZE,
					GetColor(255, 255, 0),
					TRUE
				);
			}
		}

		break;

	case LEFT:

		for (int y = -1; y <= 1; y++)
		{
			for (int x = 1; x <= 3; x++)
			{
				DrawBox(
					(enemyX - x) * CHA_SIZE,
					(enemyY + y) * CHA_SIZE,
					(enemyX - x + 1) * CHA_SIZE,
					(enemyY + y + 1) * CHA_SIZE,
					GetColor(255, 255, 0),
					TRUE
				);
			}
		}

		break;

	case UP:

		for (int y = 1; y <= 3; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				DrawBox(
					(enemyX + x) * CHA_SIZE,
					(enemyY - y) * CHA_SIZE,
					(enemyX + x + 1) * CHA_SIZE,
					(enemyY - y + 1) * CHA_SIZE,
					GetColor(255, 255, 0),
					TRUE
				);
			}
		}

		break;

	case DOWN:

		for (int y = 1; y <= 3; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				DrawBox(
					(enemyX + x) * CHA_SIZE,
					(enemyY + y) * CHA_SIZE,
					(enemyX + x + 1) * CHA_SIZE,
					(enemyY + y + 1) * CHA_SIZE,
					GetColor(255, 255, 0),
					TRUE
				);
			}
		}

		break;
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);



	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;
	int nowFrame = animFrame[frame];

	Rect iRect[4] = {
		{  nowFrame * ENEMY_SIZE, 3 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 0 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 1 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 2 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE}
	};
	DrawBox(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		GetColor(255, 255, 0), FALSE,2);
	DrawRectExtendGraph(pos_.x, pos_.y,pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		               iRect[dir_].x, iRect[dir_].y, iRect[dir_].w, iRect[dir_].h, hImage_, TRUE);
	if (animTimer < 0) {
		frame = (++frame) % 4;
		animTimer = ANIM_INTERVAL + animTimer;
	}
	animTimer = animTimer - Time::DeltaTime();
}

void Enemy::Move()
{
}

void Enemy::SerchPlayer()
{
}



void Enemy::CheckDistance()
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

	if (distance <= 5)
	{
		if (enemyY == playerY)
		{
			bool blocked = false;

			int start = min(enemyX, playerX);
			int end = max(enemyX, playerX);
			for (int x = start + 1; x < end; x++)
			{
				if (FindGameObject<Stage>()->GetMap(x, enemyY) == 1)
				{
					blocked = true;
					break;
				}
			}
			isChasing = !blocked;
			return;
		}
		if (enemyX == playerX)
		{
			bool blocked = false;
			int start = min(enemyY, playerY);
			int end = max(enemyY, playerY);
			for (int y = start + 1; y < end; y++)
			{
				if (FindGameObject<Stage>()->GetMap(enemyX, y) == 1)
				{
					blocked = true;
					break;
				}
			}
			isChasing = !blocked;
			return;
		}
	}
	isChasing = false;
}

void Enemy::Patrol()
{
	SearchPlayer();

	if (isChasing)
	{
		state_ = CHASE;
		return;
	}

	//--------------------------------
	// ランダム移動
}

void Enemy::Chase()
{
	SearchPlayer();

	if (!isChasing)
	{
		state_ = PATROL;
		return;
	}

	//--------------------------------
	// プレイヤー追跡
}

