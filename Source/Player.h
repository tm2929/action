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
	//�W�����v�U��
	void SetJumpAttackState();
	void UpdateJumpAttackState(float elapsedTime);
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
	//��
	void SetHealState();
	void UpdateHealState(float elapsedTime);
	//�X�L��
	void SetSukillState();
	void UpdateSukillState(float elapsedTime);

	//��������
	void FallOff(float elapsedTime);

	void MoveInput();
	//�ړ�,�A���O������
	void Move(float elapsedTime, float pX, float pY);
	//�U�������������A���O����ς��邽��
	void AttackChangeAngle(float elapsedTime, float pX, float pY);

	//enemy�Ƃ̋���,����,�O�ς����߂�
	void CalculateDotCrossLength();

	void HpSpMpUpdate(float elapsedTime);
	DirectX::XMFLOAT4 sukillColor = { 1,1,1,1 }; //�X�L��������J���[
	float sukillMaxColor = 2.0f;//�X�L��������J���[�}�b�N�X
	float defaultColor = 1.0f;//�ʏ�J���[
	float defaultSpeed = 1.0f;//�ʏ푬�x
private:

	float withinDistance = 100.f;
	float InputSensitivity = 0.1f; //�X�e�B�b�N����
	float attackAngleTime = 0.3f; //�U����������ς��鎞��
	//����
	std::unique_ptr<Weapon>weapon;
	//�O��
	DirectX::XMFLOAT3 trajectoryStartPos;
	DirectX::XMFLOAT3 trajectoryEndPos;
	float trajectoryLen = 150.f;//�O�Ւ���
	int trajectorynNodeNum = 47;// �O�Տo���m�[�h�ԍ�
	DirectX::XMFLOAT3 weaponPlusPos = { 0,0,0 };
	//����Data
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

	DirectX::XMFLOAT4 hitColor;//�����蔻��F
	DirectX::XMFLOAT4 hitFalseColor = { 1,1,1,1 };//�������Ă��Ȃ�
	DirectX::XMFLOAT4 hitTrueColor = { 1,0,0,1 };//�������Ă��鎞

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
		JUMPATTACK, //�W�����v�U��(���U��)
		KICK,		//�L�b�N
		ACCEL,	//���
		RUSH,		//�ːi�U��
		HIT,		//����
		KNOCKBACK,//����(�ӂ��Ƃ�)
		DEATH,
		HEAL,//��
		SUKILL,
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
		SUKILL,
		HEAL,
		CHARGE,
		SUKILL2,
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
	enum class SUKILLCOUNT
	{
		//sukill���@hit���ԊǗ�
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
