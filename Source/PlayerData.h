#pragma once
#include <DirectXMath.h>

struct PlayerHitData
{
	float damage = 0;//基礎ダメージ
	float maxDamageMagnification = 0;
	float attackPower[3] = {0,0,0};//連撃加算
	DirectX::XMFLOAT3 pulsPos = {0,0,0};//攻撃時移動距離
	float pulsDamage = 0;//連続攻撃時加算の入れ物
	float hitAttackLength = 0;//攻撃当たり判定の長さ
	float hitLength = 0;//本体当たり判定の長さ
	float hitTargetLength = 0;
	float attackTime = 0;//攻撃モーション切り替え時間
	float  count = 0;//攻撃開始フレーム
	float stopTime[4] ;//ヒットストップタイム
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
	float dashSp;//ダッシュ時消費sp
	float accelSp;//accel時消費sp
	float attackSp[3];//攻撃時消費sp
	void Init();
};
struct PlayerSpeedData
{
	float maxMoveSpeed = 0;//MaxSpeed
	float acceleration = 0;//加速
	float deceleration = 0;//減速
	//ダッシュ時
	float dashMaxMoveSpeed = 0;//MaxSpeed
	float dashAcceleration = 0;//加速
	float dashDeceleration = 0;//減速

	bool dashFlag;//ダッシュフラグ
	
	float knockBackSpeed = 0;
	float turnSpeed = DirectX::XMConvertToRadians(360);

	float accelTime = 0;
	float accelSpeed = 0;
	void Init();
};


