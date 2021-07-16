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
	//ジャンプ攻撃
	void SetJumpAttackState();
	void UpdateJumpAttackState(float elapsedTime);
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
	//回復
	void SetHealState();
	void UpdateHealState(float elapsedTime);
	//スキル
	void SetSukillState();
	void UpdateSukillState(float elapsedTime);

	//落下処理
	void FallOff(float elapsedTime);

	void MoveInput();
	//移動,アングル処理
	void Move(float elapsedTime, float pX, float pY);
	//攻撃時少しだけアングルを変えるため
	void AttackChangeAngle(float elapsedTime, float pX, float pY);

	//enemyとの距離,内積,外積を求める
	void CalculateDotCrossLength();

	void HpSpMpUpdate(float elapsedTime);
	DirectX::XMFLOAT4 sukillColor = { 1,1,1,1 }; //スキル時武器カラー
	float sukillMaxColor = 2.0f;//スキル時武器カラーマックス
	float defaultColor = 1.0f;//通常カラー
	float defaultSpeed = 1.0f;//通常速度
private:

	float withinDistance = 100.f;
	float InputSensitivity = 0.1f; //スティック入力
	float attackAngleTime = 0.3f; //攻撃時向きを変える時間
	//武器
	std::unique_ptr<Weapon>weapon;
	//軌跡
	DirectX::XMFLOAT3 trajectoryStartPos;
	DirectX::XMFLOAT3 trajectoryEndPos;
	float trajectoryLen = 150.f;//軌跡長さ
	int trajectorynNodeNum = 47;// 軌跡出現ノード番号
	DirectX::XMFLOAT3 weaponPlusPos = { 0,0,0 };
	//調整Data
	PlayerHitData			hitData;
	PlayerJumpData	   jumpData;
	PlayerSpeedData	speedData;
	PlayerEnergyData   energyData;
	PlayerSkillData		skillData;
	std::shared_ptr<PlayerObj>playerObj;

	std::vector<std::string>boonName;
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 pos = { 0,0,0 };
	DirectX::XMFLOAT3 velocity = { 0,0,0 };

	DirectX::XMFLOAT4 hitColor;//当たり判定色
	DirectX::XMFLOAT4 hitFalseColor = { 1,1,1,1 };//当たっていない
	DirectX::XMFLOAT4 hitTrueColor = { 1,0,0,1 };//当たっている時

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
		JUMPATTACK, //ジャンプ攻撃(強攻撃)
		KICK,		//キック
		ACCEL,	//回避
		RUSH,		//突進攻撃
		HIT,		//やられ
		KNOCKBACK,//やられ(ふっとび)
		DEATH,
		HEAL,//回復
		SUKILL,
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
		SUKILL,
		HEAL,
		CHARGE,
		SUKILL2,
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
	enum class SUKILLCOUNT
	{
		//sukill時　hit時間管理
		ATTACK1,
		ATTACK2,
		ATTACK3,
		ATTACK4,
		ATTACK5
	};
	SUKILLCOUNT sukillCount = SUKILLCOUNT::ATTACK1;
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
