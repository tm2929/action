#pragma once
#include <DirectXMath.h>

struct PlayerHitData
{
	float damage = 0;//��b�_���[�W
	float maxDamageMagnification = 0;
	float attackPower[3] = { 0,0,0 };//�A�����Z
	float firstAnimTime = 0; //�U��n�ߏI���܂ł̎���
	float firstAnimSpeed[2] = { 0,0 };// 0 �U��n�ߑ��x 1 ����ȍ~���x
	float secondAnimSpeed[2] = { 0,0 };
	float firstHitStartTime = 0; //�ꌂ�ڍU������������U���t���Otrue
	float secondHitStartTime = 0; //�񌂖ڍU������������U���t���Otrue
	float  thirdHitStartTime = 0; //�O���ڍU������������U���t���Otrue
	float  thirdHitEndTime = 0; //�O���ڍU������������U���t���Ofalse
	DirectX::XMFLOAT3  firstPlusPos = { 0,0,0 };//�ꌂ�ڈړ���
	DirectX::XMFLOAT3  thirdPlusPos = { 0,0,0 };//�O���ڈړ���
	DirectX::XMFLOAT3  rushPlusPos = { 0,0,0 };//rush�ړ���
	float sukillPower;//�X�L�����Z
	DirectX::XMFLOAT3 plusPos = { 0,0,0 };//�ړ���
	float plusDamage = 0;//�A���U�������Z�̓��ꕨ
	float hitAttackLength = 0;//�U�������蔻��̒���
	float hitLength = 0;//�{�̓����蔻��̒���
	float hitTargetLength = 0;
	float attackTime = 0;//�U�����[�V�����؂�ւ�����
	float  count = 0;//�U���J�n�t���[��
	float stopTime[4];//�q�b�g�X�g�b�v�^�C��
	float waiteTime = 0;
	void Init();
};
struct PlayerSkillData
{
	float healAnimTime[3]; //�񕜃A�j���[�V������������
	float stateChangeTime; //state�؂�ւ�����
	float healAnimSpeed[2]; //�񕜃A�j���[�V�������x
	float sukillAnimTime[3];//�U���X�L���A�j���[�V������������
	float sukillAnimSpeed[2];//�U���X�L���A�j���[�V�������x
	float sukillEffectTime[5]; //�U���X�L���G�t�F�N�g��������
	void Init();
};
struct PlayerJumpData
{
	float frontSpeed = 0;; //�W�����v���O�ړ����x
	float jumpAttackTime = 0; //�W�����v�U���؂�ւ�����
	float jumpAttackFlagTime = 0;//�W�����v�U���t���Otrue����
	float jumpInitialSpeed = 0;//�������x
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
	float dashSp;//�_�b�V��������sp
	float accelSp;//accel������sp
	float attackSp[3];//�U��������sp
	float tiredSp;//��Jsp
	void Init();
};
struct PlayerSpeedData
{
	float maxMoveSpeed = 0;//MaxSpeed
	float acceleration = 0;//����
	float deceleration = 0;//����
	//�_�b�V����
	float dashMaxMoveSpeed = 0;//MaxSpeed
	float dashAcceleration = 0;//����
	float dashDeceleration = 0;//����

	bool dashFlag;//�_�b�V���t���O

	float knockBackSpeed = 0;
	float turnSpeed = DirectX::XMConvertToRadians(360);

	float accelTime = 0;
	float accelSpeed = 0;
	void Init();
};


