#pragma once
#include "PlayerObj.h"
#include "PlayerData.h"
#include "Weapon.h"
#include "Trajectory.h"
#define BLENDNORMAL 5.0f
#define BLENDATTACK   15.0f //攻撃から待機に戻る間
class Player
{
public:
	Player(std::shared_ptr<ModelResource>resource, std::shared_ptr<ModelResource>weaponResource);
	void Update(float elapsedTime);
	void Imgui();
	//ゲッター
	Model* GetModel() { return playerObj->GetModel(); }
	Model* GetWeaponModel() { return weapon->GetModel(); }
	std::shared_ptr<PlayerObj> GetObj() { return playerObj; }
	PlayerHitData* GetAttackData() { return &hitData; }
	const DirectX::XMFLOAT3& GetTrajectoryStartPosition() { return trajectoryStartPos; }
	const DirectX::XMFLOAT3& GetTrajectoryEndPosition() { return trajectoryEndPos; }
	//待機
	void SetWaitState();
	void UpdateWaitState(float elapsedTime);
	//移動
	void SetRunState();
	void UpdateRunState(float elapsedTime);
	//攻撃
	void SetAttackState();
	void UpdateAttackState(float elapsedTime);
	//キック
	void SetKickState();
	void UpdateKickState(float elapsedTime);
	//突進
	void SetRushState();
	void UpdateRushState(float elapsedTime);
	//回避
	void SetAccelState();
	void UpdateAccelState(float elapsedTime);
	//やられ
	void SetHitState();
	void UpdateHitState(float elapsedTime);
	//やられ(ふっとび)
	void SetKnockBackState();
	void UpdateKnockBackState(float elapsedTime);
	//死亡
	void SetDeathState();
	void UpdateDeathState(float elapsedTime);

	void MoveInput();
	//移動,アングル処理
	void Move(float elapsedTime, float pX, float pY);
	//攻撃時少しだけアングルを変えるため
	void AttackChangeAngle(float elapsedTime, float pX, float pY);

	//enemyとの距離,内積,外積を求める
	void CalculateDotCrossLength();
private:
	//武器
	std::unique_ptr<Weapon>weapon;
	//軌跡
	DirectX::XMFLOAT3 trajectoryStartPos;
	DirectX::XMFLOAT3 trajectoryEndPos;

	DirectX::XMFLOAT3 weaponPlusPos = { 0,0,0 };
	//調整Data
	PlayerHitData hitData;
	PlayerSpeedData	speedData;
	PlayerEnergyData   energyData;
	std::shared_ptr<PlayerObj>playerObj;


	std::vector<std::string>boonName;
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 pos = { 0,0,0 };
	DirectX::XMFLOAT3 velocity = { 0,0,0 };

	DirectX::XMFLOAT4 color[3];//当たり判定色

	DirectX::XMVECTOR vec;
	float len = 0.f;					 //距離
	float dot = 0.f;					 //内積
	DirectX::XMFLOAT3 cross = { 0,0,0 }; //外積

	float power = 0.f;
	float vx = 0.f;
	float vy = 0.f;
	float vz = 0.f;

	float count = 0;

	float powerX = .0f;
	float powerY = .0f;

	bool keyFlag = false;
	bool lerpFlag = false;
	float tst = 60.0f;
	enum class STATE
	{
		WAIT,		//待機
		RUN,		//移動
		ATTACK,	//攻撃
		KICK,		//キック
		ACCEL,	//回避
		RUSH,		//突進攻撃
		HIT,		//やられ
		KNOCKBACK,//やられ(ふっとび)
		DEATH,
		TST,	//tst
	}; STATE state;
	STATE prevState = state;//前STATE

#if 0
	enum class ANIM
	{
		WAIT,
		RUN,
		ATTACK1,
		ATTACK1END,
		ATTACK2,
		ATTACK2END,
		ATTACK3,
		ATTACK3END,
		ACCEL,
		TST,
		HIT,
		KNOCKBACK1,
		KNOCKBACK2,
		UP,
		ACCELATTACK1,
		ACCELATTACK2,
	};
#else 
	enum class ANIM
	{
		WAIT1,
		WAIT2,
		WAIT3,
		RUN1,
		RUN,
		ATTACK1,
		ATTACK2,
		ATTACK3,
		HIT1,
		HIT2,
		KNOCKBACK,
		ACCEL,
		DIE,
		ATTACK4,
		KICK,
		END
	};
#endif 
	int animNo = 0;
	//アニメーション
	float animSpeed = 1.0f;;//アニメーション速度調整用
	float animSpeeds[static_cast<int>(ANIM::END)];
	float animTime = 0.f;//アニメーション時間
	bool animStopFlag;//アニメーションストップ用フラグ
	void LoadAnimSpeed();
	void SaveAnimSpeed();
	enum class ACTION
	{
		FIRST,
		FIRSTEND,
		SECOND,
		SECONDEND,
		THIRD,
		THIRDEND,
	};
	ACTION action = ACTION::FIRST;
};
