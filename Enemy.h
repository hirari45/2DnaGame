#pragma once
#include ".\Library\GameObject.h"
#include "global.h"

enum EnemyState
{
	PATROL,
	CHASE
};

class Enemy :
    public GameObject
{
	int hImage_;//‰æ‘œID
	Point pos_;//ˆÊ’u
	DIR dir_;//ˆÚ“®•ûŒü

	EnemyState state_;
	float detectRange;
	bool isChasing;

public:
	Enemy();
	~Enemy();
	void Update() override;
	void Draw() override;

	void Move();
	void SearchPlayer();
	void CheckDistance();
	void Patrol();
	void Chase();
};

