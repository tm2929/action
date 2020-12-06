#include "EnemyData.h"


void EnemyHitData::Init()
{
	damage = 5.f;

	attackPower[0] = 0.f;//ˆêŒ‚–Ú
	attackPower[1] = 5.f;//“ñŒ‚–Ú
	attackPower[2] = 10.f;//‚RŒ‚–Ú

	pulsPos = { 10.f,10.f,10.f };

	hitLength = 6.0f;
	hitAttackLength = 12.0f;

	pulsDamage = 0.f;

	attackTime = 0.5f;

	count = 0;
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
