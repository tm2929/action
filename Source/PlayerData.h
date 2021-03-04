#pragma once
#include <DirectXMath.h>

struct PlayerHitData
{
	float damage = 0;//��b�_���[�W
	float maxDamageMagnification = 0;
	float attackPower[3] = {0,0,0};//�A�����Z
	DirectX::XMFLOAT3 pulsPos = {0,0,0};//�U�����ړ�����
	float pulsDamage = 0;//�A���U�������Z�̓��ꕨ
	float hitAttackLength = 0;//�U�������蔻��̒���
	float hitLength = 0;//�{�̓����蔻��̒���
	float hitTargetLength = 0;
	float attackTime = 0;//�U�����[�V�����؂�ւ�����
	float  count = 0;//�U���J�n�t���[��
	float stopTime[4] ;//�q�b�g�X�g�b�v�^�C��
	float waiteTime = 0;
	void Init();
};
struct PlayerEnergyData
{
	float hp;
	float	maxHp;
	float sp;
	float maxSp;
	float pulsSp;
	float dashSp;//�_�b�V��������sp
	float accelSp;//accel������sp
	float attackSp[3];//�U��������sp
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


