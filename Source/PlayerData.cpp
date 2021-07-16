#include "PlayerData.h"

void PlayerHitData::Init()
{
	damage = 1.f;

	attackPower[0] = 0.5f;//一撃目
	attackPower[1] = 0.8f;//二撃目
	attackPower[2] = 1.f;//３撃目
	sukillPower = 1.4f;
	firstAnimTime = 0.4f; //振り始め

	firstAnimSpeed[0] = 2.0f;//振り始め
	firstAnimSpeed[1] = 1.5f;//それ以降
	secondAnimSpeed[0] = 0.8f;
	secondAnimSpeed[1] = 1.0f;

	firstHitStartTime = 0.1f; //一撃目
	secondHitStartTime = 0.4f;//二撃目
	thirdHitStartTime = 0.4f;//三撃目
	thirdHitEndTime = 0.8f;//三撃目

	stopTime[0] = 2.f;//一撃目
	stopTime[1] = 4.f;//二撃目
	stopTime[2] = 6.f;//三撃目
	stopTime[3] = stopTime[0];//器

	plusPos = { 0.f,0.f,0.f };

	firstPlusPos = { 30.f, 0.f, 30.f };
	thirdPlusPos = { 20.f,0.f,20.f };
	rushPlusPos = { 100.f,0.f,100.f };
	hitLength = 2.0f;
	hitAttackLength = 3.5f;
	hitTargetLength = 2.0f;
	plusDamage = 0.f;

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
	accelTime = 2.f;

	knockBackSpeed = 100.f;

}

void PlayerEnergyData::Init()
{
	hp = 100;
	maxHp = 100;
	sp = 100;
	maxSp = 100;
	mp = 0;
	maxMp = 100;
	plusMp = 6.5f;
	plusSp = 15.f;
	plusHp = 4.f;
	dashSp = 40.0f;
	accelSp = 40.0f;
	attackSp[0] = 2.f;
	attackSp[1] = 5.f;
	attackSp[2] = 5.f;
	tiredSp = 30.f;
}

void PlayerSkillData::Init()
{
	//構えるまでの時間
	healAnimTime[0] = 0.4f;
	healAnimSpeed[0] = 1.0f;
	//完全ストップまでの時間
	healAnimTime[1] = 1.5f;
	healAnimSpeed[1] = 0.7f;
	//終わり
	healAnimTime[2] = 1.7f;
	//state切り替え時間
	stateChangeTime = 0.2f;
	//構えるまでの時間
	sukillAnimTime[0] = 0.4f;
	sukillAnimSpeed[0] = 1.0f;
	//完全ストップまでの時間
	sukillAnimTime[1] = 1.2f;
	sukillAnimSpeed[1] = 0.7f;
	//終わり
	sukillAnimTime[2] = 1.7f;

	//sukillEffect
	sukillEffectTime[0] = 2.4f;
	sukillEffectTime[1] = 0.8f;
	sukillEffectTime[2] = 0.8f;
	sukillEffectTime[3] = 0.5f;
	sukillEffectTime[4] = 3.0f;
}

void PlayerJumpData::Init()
{
	frontSpeed = 120.f;
	jumpInitialSpeed = 1.5f;
	jumpAttackTime = 0.5f;
	gravity = -9.8f;
	hitGravity = 30.f;
}
