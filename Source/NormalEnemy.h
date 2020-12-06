#pragma once
#include "EnemyObj.h"
#include "EnemyData.h"

class NormalEnemy
{
public:
	NormalEnemy(std::shared_ptr<ModelResource>resource);
	void Update(float elapsedTime);
	void Imgui();
	enum class STATE
	{
		CHOICE,		//選択
		WAIT,		//待機
		RUN,		//移動
		ATTACK,		//攻撃
		SEARCH,		//索敵
		KNOCKBACK,  //ノックバック
	}; STATE state;

	enum class ANIM
	{
		WAIT,
	};
	//ゲッター
	Model* GetModel() { return enemyObj->GetModel(); }
	std::shared_ptr<EnemyObj> GetObj() { return enemyObj; }

	//待機
	void SetWaitState();
	void UpdateWaitState(float elapsedTime);
	//移動
	void SetRunState();
	void UpdateRunState(float elapsedTime);
	//攻撃
	void SetAttackState();
	void UpdateAttackState(float elapsedTime);
	//ノックバック
	void SetKnockBackState();
	void UpdateKnockBackState(float elapsedTime);
private:
	std::shared_ptr<EnemyObj>enemyObj;
	EnemyHitData hitData;
	int animNo = 0;
	DirectX::XMFLOAT4 color[3];//当たり判定色


};