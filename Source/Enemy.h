#pragma once
#include "EnemyObj.h"
#include "EnemyData.h"
#include <random>

class Enemy
{
public:
	Enemy(std::shared_ptr<ModelResource>resource);
	void Update(float elapsedTime);
	void SaveData();
	void LoadData();
	void Imgui();
	//ゲッター
	Model* GetModel() { return enemyObj->GetModel(); }
	std::shared_ptr<EnemyObj> GetObj() { return enemyObj; }
	EnemyThunderData GetThunderData() { return thunderData; }
	std::shared_ptr<Character> GetRAttackObj1() { return rAttackObj1; }
	float GetEffectColorW1()const { return rAttackData.effectColorW1; }
	std::shared_ptr<Character> GetRAttackObj2() { return rAttackObj2; }
	float GetEffectColorW2()const { return rAttackData.effectColorW1; }
	std::shared_ptr<Character> GetSkull() { return skull; }

	bool attackStartFlag = false;//戦闘開始フラグ
private:
	enum class STATE
	{
		FIRST,
		CHOICE,		//選択
		WAIT,		//待機
		SIDE,		//横移動
		RUN,		//移動
		BACK,
		SHORTATTACK,//近攻撃
		LONGATTACK, //遠攻撃
		KNOCKBACK,  //ノックバック
		TELEPORTATION,//瞬間移動
		FALL,//obj落下
		DOWN,
		PLAYERSUKILLDOWN,//プレイヤースキル時
		DEATH

	}; STATE state;
	enum class ANIM
	{
		WAIT,
		RUN,//前かがみ

		FATTACK1,//振り上げ
		FATTACK2,//振り上げマックス
		FATTACK3,//振り下ろし

		SATTACK,//２連撃

		RATTACK1,//回転攻撃入り
		RATTACK2,//回転攻撃
		RATTACK3,//回転攻撃終わり

		UATTACK,//上振り攻撃

		R2ATTACK1,//回転攻撃2ため
		R2ATTACK2,//回転攻撃2マックス
		R2ATTACK3, //回転攻撃振り下ろし

		SHOT1,//ショットはじめ
		SHOT2,//ショットマックス
		SHOT3,//ショット放つ

		HANDUP1,//左手はじめ
		HANDUP2,//左手マックス
		HANDUP3,//左手下ろす

		DAMAGE1,//やられ
		DAMAGE2,//前倒れ
		DAMAGE3,//戻り
	};
	void SAttack(float elapsedTime);//２連撃
	void RAttack(float elpasedTime);//回転攻撃
	void UAttack(float elpasedTime);//上振り攻撃
	void RushAttack(float elapsedTime);//突進
	void ThunderAttack(float elapsedTime);//雷
	void ShotAttack(float elapsedTime);//ショット攻撃
	//選択
	void SetChoiceState();
	void UpdateChoiceState(float elapsedTime);
	//待機
	void SetWaitState();
	void UpdateWaitState(float elapsedTime);
	//横移動
	void SetSideStepState();
	void UpdatSideStepState(float elapsedTime);
	//移動
	void SetRunState();
	void UpdateRunState(float elapsedTime);
	//移動(後ろ)
	void SetBackStepState();
	void UpdateBackStepState(float elapsedTime);
	//攻撃
	void SetShortAttackState();
	void UpdateShortAttackState(float elapsedTime);
	void SetLongAttackState();
	void UpdateLongAttackState(float elapsedTime);
	// 瞬間移動
	void SetTeleportationState();
	void UpdateTeleportationState(float elapsedTime);
	//回避
	void SetAccelState();
	void UpdateAccelState(float elapsedTime);
	//ノックバック
	void SetKnockBackState();
	void UpdateKnockBackState(float elapsedTime);
	//戦闘開始時挙動
	void SetFirstState();
	void UpdateFirstState(float elapsedTime);
	//down
	void SetDownState();
	void UpdateDownState(float elapsedTime);
	//playerスキルヒット時	
	void SetPlayerSukillDownState();
	void UpdatePlayerSukillDownState(float elapsedTime);
	//死亡
	void SetDeathState();
	void UpdateDeathState(float elapsedTime);
	enum  SHORTATTACK
	{
		SATTACK,//2連
		RATTACK,//回転
		UATTACK,//振り上げ
	};
	enum  LONGATTACK
	{
		RUSH,//突進
		THUNDER,//雷
		SHOT,//ショット
	};
	enum class ACTION
	{
		FIRST,
		FIRSTEND,
		SECOND,
		SECONDEND,
		THIRD,
	};
	ACTION actionState;
	//enemyで使うエフェクト(mist以外)停止
	void EffectStop();
	//playerとの内積,外積を求める
	void CalculateDotCross();
	//playerとの距離を求める
	void CalculateLength();
private:
	bool debugFlag = false;//各stateを自分で呼ぶ用
	float power = 0.f;
	float animSpeed = 1.0f;;//アニメーション速度調整用
	float animTime = 0.f;//アニメーション時間
	bool animStopFlag;//アニメーションストップ用フラグ
	DirectX::XMFLOAT3 pos = { 0,0,0 };
	DirectX::XMFLOAT3 playerPos = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	std::shared_ptr<EnemyObj>enemyObj;
	//RATTACK時エフェクト
	std::shared_ptr<Character>rAttackObj1;
	std::shared_ptr<Character>rAttackObj2;


	std::shared_ptr<Character>skull;
	EnemyHitData	hitData;
	EnemyChoiceData choiceData;
	EnemyAttackData attackData;
	EnemySpeedData  speedData;
	EnemyThunderData thunderData;
	EnemyShotData shotData;
	EnemyRAttackData rAttackData;
	int animNo = 0;
	DirectX::XMFLOAT4 color[3];//当たり判定色
	float waiteTime;
	float len;
	DirectX::XMVECTOR vec;
	DirectX::XMVECTOR dir;
	float dot;
	DirectX::XMFLOAT3 cross;
	bool attackMoveFlag = false;//ひとつ前のstateが攻撃ならtrue
	int   number;
	int   beforeNumber;//choice時numを保存
	//ショット時
	float shotAngle = 0;
	float shotTime = 0;
};
