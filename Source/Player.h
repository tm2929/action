#pragma once
#include "PlayerObj.h"
#include "PlayerData.h"
#include "Weapon.h"
#include "Trajectory.h"
#define BLENDNORMAL 5.0f
#define BLENDATTACK   15.0f //�U������ҋ@�ɖ߂��
class Player
{
public:
	Player(std::shared_ptr<ModelResource>resource, std::shared_ptr<ModelResource>weaponResource);
	void Update(float elapsedTime);
	void Imgui();
	//�Q�b�^�[
	Model* GetModel() { return playerObj->GetModel(); }
	Model* GetWeaponModel() { return weapon->GetModel(); }
	std::shared_ptr<PlayerObj> GetObj() { return playerObj; }
	PlayerHitData* GetAttackData() { return &hitData; }
	const DirectX::XMFLOAT3& GetTrajectoryStartPosition() { return trajectoryStartPos; }
	const DirectX::XMFLOAT3& GetTrajectoryEndPosition() { return trajectoryEndPos; }
	//�ҋ@
	void SetWaitState();
	void UpdateWaitState(float elapsedTime);
	//�ړ�
	void SetRunState();
	void UpdateRunState(float elapsedTime);
	//�U��
	void SetAttackState();
	void UpdateAttackState(float elapsedTime);
	//�L�b�N
	void SetKickState();
	void UpdateKickState(float elapsedTime);
	//�ːi
	void SetRushState();
	void UpdateRushState(float elapsedTime);
	//���
	void SetAccelState();
	void UpdateAccelState(float elapsedTime);
	//����
	void SetHitState();
	void UpdateHitState(float elapsedTime);
	//����(�ӂ��Ƃ�)
	void SetKnockBackState();
	void UpdateKnockBackState(float elapsedTime);
	//���S
	void SetDeathState();
	void UpdateDeathState(float elapsedTime);

	void MoveInput();
	//�ړ�,�A���O������
	void Move(float elapsedTime, float pX, float pY);
	//�U�������������A���O����ς��邽��
	void AttackChangeAngle(float elapsedTime, float pX, float pY);

	//enemy�Ƃ̋���,����,�O�ς����߂�
	void CalculateDotCrossLength();
private:
	//����
	std::unique_ptr<Weapon>weapon;
	//�O��
	DirectX::XMFLOAT3 trajectoryStartPos;
	DirectX::XMFLOAT3 trajectoryEndPos;

	DirectX::XMFLOAT3 weaponPlusPos = { 0,0,0 };
	//����Data
	PlayerHitData hitData;
	PlayerSpeedData	speedData;
	PlayerEnergyData   energyData;
	std::shared_ptr<PlayerObj>playerObj;


	std::vector<std::string>boonName;
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 pos = { 0,0,0 };
	DirectX::XMFLOAT3 velocity = { 0,0,0 };

	DirectX::XMFLOAT4 color[3];//�����蔻��F

	DirectX::XMVECTOR vec;
	float len = 0.f;					 //����
	float dot = 0.f;					 //����
	DirectX::XMFLOAT3 cross = { 0,0,0 }; //�O��

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
		WAIT,		//�ҋ@
		RUN,		//�ړ�
		ATTACK,	//�U��
		KICK,		//�L�b�N
		ACCEL,	//���
		RUSH,		//�ːi�U��
		HIT,		//����
		KNOCKBACK,//����(�ӂ��Ƃ�)
		DEATH,
		TST,	//tst
	}; STATE state;
	STATE prevState = state;//�OSTATE

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
	//�A�j���[�V����
	float animSpeed = 1.0f;;//�A�j���[�V�������x�����p
	float animSpeeds[static_cast<int>(ANIM::END)];
	float animTime = 0.f;//�A�j���[�V��������
	bool animStopFlag;//�A�j���[�V�����X�g�b�v�p�t���O
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
