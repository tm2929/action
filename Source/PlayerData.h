#pragma once
#include <DirectXMath.h>

struct PlayerHitData
{
	float damage = 0;//îb_[W
	float maxDamageMagnification = 0;
	float attackPower[3] = {0,0,0};//AÁZ
	DirectX::XMFLOAT3 pulsPos = {0,0,0};//UÚ®£
	float pulsDamage = 0;//A±UÁZÌüê¨
	float hitAttackLength = 0;//U½è»èÌ·³
	float hitLength = 0;//{Ì½è»èÌ·³
	float hitTargetLength = 0;
	float attackTime = 0;//U[VØèÖ¦Ô
	float  count = 0;//UJnt[
	float stopTime[4] ;//qbgXgbv^C
	float waiteTime = 0;
	void Init();
};
struct PlayerEnergyData
{
	float hp;
	float	maxHp;
	float sp;
	float maxSp;
	float pulsSp;
	float dashSp;//_bVÁïsp
	float accelSp;//accelÁïsp
	float attackSp[3];//UÁïsp
	void Init();
};
struct PlayerSpeedData
{
	float maxMoveSpeed = 0;//MaxSpeed
	float acceleration = 0;//Á¬
	float deceleration = 0;//¸¬
	//_bV
	float dashMaxMoveSpeed = 0;//MaxSpeed
	float dashAcceleration = 0;//Á¬
	float dashDeceleration = 0;//¸¬

	float knockBackSpeed = 0;
	float turnSpeed = DirectX::XMConvertToRadians(360);

	float accelTime = 0;
	float accelSpeed = 0;
	void Init();
};


