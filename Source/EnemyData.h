#pragma once
#include <DirectXMath.h>
#include "HitObjData.h"
#define THUNDERNUM 1
struct EnemyHitData
{
	float damage;//基礎ダメージ
	float maxDamageMagnification;
	float attackPower[3];//連撃加算
	DirectX::XMFLOAT3 plusPos;//攻撃時移動距離
	float plusDamage;//連続攻撃時加算の入れ物
	float hitAttackLength;//攻撃当たり判定の長さ
	float hitLength;//本体当たり判定の長さ
	float attackTime;//攻撃モーション切り替え時間
	int count = 0;//攻撃開始フレーム

	float downTime;//down時間
	float sukillDownTime;//sukill時のdown時間
	void Init();
};
struct EnemyChoiceData
{
	float waiteTime = 0;//待機時間
	float time = 0;
	int num = 0;//debug時、攻撃number変更用
	float choiceLen[3];	//攻撃選択距離
	bool firstFlag=false; //一番最初はプレイヤーがある程度近づいてからボス戦をはじめるように
	float firstArea = 0; //戦闘開始距離

	void Init();
};
struct EnemyShotData
{
	Sphere sphere;//当たり判定
	
	float area;
	bool exist;
	DirectX::XMFLOAT3 scale;
	float y;
	float moveTime;//enemy本体移動時間
	float time[2]; //0角度変更時間 1ショット時間
	float speed;
	void Init();
};
struct EnemyThunderData
{
	DirectX::XMFLOAT3 pos[THUNDERNUM];
	DirectX::XMFLOAT3 playerPos[THUNDERNUM];
	float w[THUNDERNUM];
	bool exist;
	Cylinder cylinder;//当たり判定
	float area;
	float attackStartTime;//当たり判定start
	float attackEndTime;//当たり判定end
	float endTime;//state終了
	void Init();
};
struct EnemyAttackData
{
	float angleTime;//向き変更時間
	float rushTime;
	float chargeTime;//ため時間
	bool rushFlag[3];
	void Init();
};
struct EnemyRAttackData
{
	//器
	DirectX::XMFLOAT3 effectScale1 = {0,0,0};
	DirectX::XMFLOAT3 effectScale2 = { 0,0,0 };
	float effectAngle1 = 0;
	float effectAngle2 = 0;

	DirectX::XMFLOAT3 defaultScale1;
	DirectX::XMFLOAT3 defaultScale2;
	float dispMaxTimer;
	float easingScale1 ;
	float easingScale2 ;
	float easingColor ;
	float effectColorW1;
	float effectColorW2 ;
	void Init();
};
struct EnemySpeedData
{
	float accelSpeed;
	float runSpeed;

	void Init();
};