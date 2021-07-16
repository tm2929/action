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
	//�Q�b�^�[
	Model* GetModel() { return enemyObj->GetModel(); }
	std::shared_ptr<EnemyObj> GetObj() { return enemyObj; }
	EnemyThunderData GetThunderData() { return thunderData; }
	std::shared_ptr<Character> GetRAttackObj1() { return rAttackObj1; }
	float GetEffectColorW1()const { return rAttackData.effectColorW1; }
	std::shared_ptr<Character> GetRAttackObj2() { return rAttackObj2; }
	float GetEffectColorW2()const { return rAttackData.effectColorW1; }
	std::shared_ptr<Character> GetSkull() { return skull; }

	bool attackStartFlag = false;//�퓬�J�n�t���O
private:
	enum class STATE
	{
		FIRST,
		CHOICE,		//�I��
		WAIT,		//�ҋ@
		SIDE,		//���ړ�
		RUN,		//�ړ�
		BACK,
		SHORTATTACK,//�ߍU��
		LONGATTACK, //���U��
		KNOCKBACK,  //�m�b�N�o�b�N
		TELEPORTATION,//�u�Ԉړ�
		FALL,//obj����
		DOWN,
		PLAYERSUKILLDOWN,//�v���C���[�X�L����
		DEATH

	}; STATE state;
	enum class ANIM
	{
		WAIT,
		RUN,//�O������

		FATTACK1,//�U��グ
		FATTACK2,//�U��グ�}�b�N�X
		FATTACK3,//�U�艺�낵

		SATTACK,//�Q�A��

		RATTACK1,//��]�U������
		RATTACK2,//��]�U��
		RATTACK3,//��]�U���I���

		UATTACK,//��U��U��

		R2ATTACK1,//��]�U��2����
		R2ATTACK2,//��]�U��2�}�b�N�X
		R2ATTACK3, //��]�U���U�艺�낵

		SHOT1,//�V���b�g�͂���
		SHOT2,//�V���b�g�}�b�N�X
		SHOT3,//�V���b�g����

		HANDUP1,//����͂���
		HANDUP2,//����}�b�N�X
		HANDUP3,//���艺�낷

		DAMAGE1,//����
		DAMAGE2,//�O�|��
		DAMAGE3,//�߂�
	};
	void SAttack(float elapsedTime);//�Q�A��
	void RAttack(float elpasedTime);//��]�U��
	void UAttack(float elpasedTime);//��U��U��
	void RushAttack(float elapsedTime);//�ːi
	void ThunderAttack(float elapsedTime);//��
	void ShotAttack(float elapsedTime);//�V���b�g�U��
	//�I��
	void SetChoiceState();
	void UpdateChoiceState(float elapsedTime);
	//�ҋ@
	void SetWaitState();
	void UpdateWaitState(float elapsedTime);
	//���ړ�
	void SetSideStepState();
	void UpdatSideStepState(float elapsedTime);
	//�ړ�
	void SetRunState();
	void UpdateRunState(float elapsedTime);
	//�ړ�(���)
	void SetBackStepState();
	void UpdateBackStepState(float elapsedTime);
	//�U��
	void SetShortAttackState();
	void UpdateShortAttackState(float elapsedTime);
	void SetLongAttackState();
	void UpdateLongAttackState(float elapsedTime);
	// �u�Ԉړ�
	void SetTeleportationState();
	void UpdateTeleportationState(float elapsedTime);
	//���
	void SetAccelState();
	void UpdateAccelState(float elapsedTime);
	//�m�b�N�o�b�N
	void SetKnockBackState();
	void UpdateKnockBackState(float elapsedTime);
	//�퓬�J�n������
	void SetFirstState();
	void UpdateFirstState(float elapsedTime);
	//down
	void SetDownState();
	void UpdateDownState(float elapsedTime);
	//player�X�L���q�b�g��	
	void SetPlayerSukillDownState();
	void UpdatePlayerSukillDownState(float elapsedTime);
	//���S
	void SetDeathState();
	void UpdateDeathState(float elapsedTime);
	enum  SHORTATTACK
	{
		SATTACK,//2�A
		RATTACK,//��]
		UATTACK,//�U��グ
	};
	enum  LONGATTACK
	{
		RUSH,//�ːi
		THUNDER,//��
		SHOT,//�V���b�g
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
	//enemy�Ŏg���G�t�F�N�g(mist�ȊO)��~
	void EffectStop();
	//player�Ƃ̓���,�O�ς����߂�
	void CalculateDotCross();
	//player�Ƃ̋��������߂�
	void CalculateLength();
private:
	bool debugFlag = false;//�estate�������ŌĂԗp
	float power = 0.f;
	float animSpeed = 1.0f;;//�A�j���[�V�������x�����p
	float animTime = 0.f;//�A�j���[�V��������
	bool animStopFlag;//�A�j���[�V�����X�g�b�v�p�t���O
	DirectX::XMFLOAT3 pos = { 0,0,0 };
	DirectX::XMFLOAT3 playerPos = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	std::shared_ptr<EnemyObj>enemyObj;
	//RATTACK���G�t�F�N�g
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
	DirectX::XMFLOAT4 color[3];//�����蔻��F
	float waiteTime;
	float len;
	DirectX::XMVECTOR vec;
	DirectX::XMVECTOR dir;
	float dot;
	DirectX::XMFLOAT3 cross;
	bool attackMoveFlag = false;//�ЂƂO��state���U���Ȃ�true
	int   number;
	int   beforeNumber;//choice��num��ۑ�
	//�V���b�g��
	float shotAngle = 0;
	float shotTime = 0;
};
