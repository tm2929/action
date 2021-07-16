#include "EnemyData.h"


void EnemyHitData::Init()
{
	damage = 5.f;
	attackPower[0] = 0.f;//ˆêŒ‚–Ú
	attackPower[1] = 5.f;//“ñŒ‚–Ú
	attackPower[2] = 10.f;//‚RŒ‚–Ú

	plusPos = { 10.f,10.f,10.f };

	hitLength = 6.0f;
	hitAttackLength = 12.0f;

	plusDamage = 0.f;

	attackTime = 0.5f;

	count = 0;

	downTime = 3.f;
	sukillDownTime = 6.f;
}

void EnemyChoiceData::Init()
{

	waiteTime = 2.f;
	choiceLen[0] = 70.0f; choiceLen[1] = 180.0f; choiceLen[2] = 200;

	firstFlag = true;
	firstArea = 40;

	time = 1.f;
	num = 2;
}

void EnemyAttackData::Init()
{
	angleTime = 0.6f;
	rushTime = 0.6f;
	chargeTime = 1.5f;
	for (int i = 0; i < 3; i++)
	{
		rushFlag[i] = false;
	}
}

void EnemySpeedData::Init()
{
	accelSpeed = 200.f;
	runSpeed = 60.f;
}

void EnemyShotData::Init()
{
	area = 13.f;
	scale = DirectX::XMFLOAT3(0.3f, 0.3f, 0.3f);
	y = -20;
	time[0] = 0.3f;
	time[1] = 2.0f;
	moveTime = 1.5f;
	speed = 250.f;
	exist = true;
}
void EnemyThunderData::Init()
{
	for (int i = 0; i < THUNDERNUM; i++)
	{
		pos[i] = { 0,0,0 };
		w[i] = 0;
	}
	area = 17;
	exist = false;
	attackStartTime = 0.2f;
	attackEndTime = 0.9f;
	endTime = 2.0f;
}

void EnemyRAttackData::Init()
{
	dispMaxTimer = 0.5f;
	easingScale1 = 30;
	easingScale2 = 30;
	easingColor = 4.f;
	effectColorW1 = 1;
	effectColorW2 = 1;
	defaultScale1 = DirectX::XMFLOAT3(0, 3.5f, 0);
	defaultScale2 = DirectX::XMFLOAT3(0, 3.5f, 0);
}
