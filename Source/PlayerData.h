#pragma once
#include <DirectXMath.h>

struct PlayerHitData
{
	float damage = 0;//基礎ダメージ
	float maxDamageMagnification = 0;
	float attackPower[3] = { 0,0,0 };//連撃加算
	float firstAnimTime = 0; //振り始め終わるまでの時間
	float firstAnimSpeed[2] = { 0,0 };// 0 振り始め速度 1 それ以降速度
	float secondAnimSpeed[2] = { 0,0 };
	float firstHitStartTime = 0; //一撃目攻撃時超えたら攻撃フラグtrue
	float secondHitStartTime = 0; //二撃目攻撃時超えたら攻撃フラグtrue
	float  thirdHitStartTime = 0; //三撃目攻撃時超えたら攻撃フラグtrue
	float  thirdHitEndTime = 0; //三撃目攻撃時超えたら攻撃フラグfalse
	DirectX::XMFLOAT3  firstPlusPos = { 0,0,0 };//一撃目移動量
	DirectX::XMFLOAT3  thirdPlusPos = { 0,0,0 };//三撃目移動量
	DirectX::XMFLOAT3  rushPlusPos = { 0,0,0 };//rush移動量
	float sukillPower;//スキル加算
	DirectX::XMFLOAT3 plusPos = { 0,0,0 };//移動量
	float plusDamage = 0;//連続攻撃時加算の入れ物
	float hitAttackLength = 0;//攻撃当たり判定の長さ
	float hitLength = 0;//本体当たり判定の長さ
	float hitTargetLength = 0;
	float attackTime = 0;//攻撃モーション切り替え時間
	float  count = 0;//攻撃開始フレーム
	float stopTime[4];//ヒットストップタイム
	float waiteTime = 0;
	void Init();
};
struct PlayerSkillData
{
	float healAnimTime[3]; //回復アニメーション調整時間
	float stateChangeTime; //state切り替え時間
	float healAnimSpeed[2]; //回復アニメーション速度
	float sukillAnimTime[3];//攻撃スキルアニメーション調整時間
	float sukillAnimSpeed[2];//攻撃スキルアニメーション速度
	float sukillEffectTime[5]; //攻撃スキルエフェクト調整時間
	void Init();
};
struct PlayerJumpData
{
	float frontSpeed = 0;; //ジャンプ中前移動速度
	float jumpAttackTime = 0; //ジャンプ攻撃切り替え時間
	float jumpAttackFlagTime = 0;//ジャンプ攻撃フラグtrue時間
	float jumpInitialSpeed = 0;//初期速度
	float gravity = 0;
	float hitGravity = 0;
	void Init();
};
struct PlayerEnergyData
{
	float hp;
	float	maxHp;
	float sp;
	float maxSp;
	float mp;
	float maxMp;
	float plusMp;
	float plusSp;
	float plusHp;
	float dashSp;//ダッシュ時消費sp
	float accelSp;//accel時消費sp
	float attackSp[3];//攻撃時消費sp
	float tiredSp;//疲労sp
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


