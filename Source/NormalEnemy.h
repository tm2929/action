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
		CHOICE,		//�I��
		WAIT,		//�ҋ@
		RUN,		//�ړ�
		ATTACK,		//�U��
		SEARCH,		//���G
		KNOCKBACK,  //�m�b�N�o�b�N
	}; STATE state;

	enum class ANIM
	{
		WAIT,
	};
	//�Q�b�^�[
	Model* GetModel() { return enemyObj->GetModel(); }
	std::shared_ptr<EnemyObj> GetObj() { return enemyObj; }

	//�ҋ@
	void SetWaitState();
	void UpdateWaitState(float elapsedTime);
	//�ړ�
	void SetRunState();
	void UpdateRunState(float elapsedTime);
	//�U��
	void SetAttackState();
	void UpdateAttackState(float elapsedTime);
	//�m�b�N�o�b�N
	void SetKnockBackState();
	void UpdateKnockBackState(float elapsedTime);
private:
	std::shared_ptr<EnemyObj>enemyObj;
	EnemyHitData hitData;
	int animNo = 0;
	DirectX::XMFLOAT4 color[3];//�����蔻��F


};