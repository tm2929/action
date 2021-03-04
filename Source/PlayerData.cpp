#include "PlayerData.h"

void PlayerHitData::Init()
{
	damage = 1.f;

	attackPower[0] = 0.5f;//ˆêŒ‚–Ú
	attackPower[1] = 0.8f;//“ñŒ‚–Ú
	attackPower[2] = 1.f;//‚RŒ‚–Ú

	stopTime[0] = 2.f;//ˆêŒ‚–Ú
	stopTime[1] = 4.f;//“ñŒ‚–Ú
	stopTime[2] = 6.f;//‚RŒ‚–Ú
	stopTime[3] = stopTime[0];//Ší

	pulsPos = { 10.f,10.f,10.f };

	hitLength = 2.0f;
	hitAttackLength = 3.5f;
	hitTargetLength = 2.0f;
	pulsDamage = 0.f;

	attackTime = 0.5f;

	count = 0.f;

	waiteTime = 0.f;
}

void PlayerSpeedData::Init()
{
	maxMoveSpeed = 60.f;
	acceleration = 40.f;
	deceleration = 20.f;

	dashMaxMoveSpeed = 90.f;
	dashAcceleration = 40.f;
	dashDeceleration = 20.f;
	dashFlag = false;
	accelSpeed = 160.f;
	accelTime = 0.45f;

	knockBackSpeed = 100.f;

}

void PlayerEnergyData::Init()
{
	hp = 100;
	maxHp = 100;
	sp = 100;
	maxSp = 100;
	pulsSp = 15.f;
	dashSp = 30.0f;
	accelSp = 20.0f;
	attackSp[0] = 2.f;
	attackSp[1] = 5.f;
	attackSp[2] = 5.f;
}
