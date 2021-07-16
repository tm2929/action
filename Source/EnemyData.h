#pragma once
#include <DirectXMath.h>
#include "HitObjData.h"
#define THUNDERNUM 1
struct EnemyHitData
{
	float damage;//��b�_���[�W
	float maxDamageMagnification;
	float attackPower[3];//�A�����Z
	DirectX::XMFLOAT3 plusPos;//�U�����ړ�����
	float plusDamage;//�A���U�������Z�̓��ꕨ
	float hitAttackLength;//�U�������蔻��̒���
	float hitLength;//�{�̓����蔻��̒���
	float attackTime;//�U�����[�V�����؂�ւ�����
	int count = 0;//�U���J�n�t���[��

	float downTime;//down����
	float sukillDownTime;//sukill����down����
	void Init();
};
struct EnemyChoiceData
{
	float waiteTime = 0;//�ҋ@����
	float time = 0;
	int num = 0;//debug���A�U��number�ύX�p
	float choiceLen[3];	//�U���I������
	bool firstFlag=false; //��ԍŏ��̓v���C���[��������x�߂Â��Ă���{�X����͂��߂�悤��
	float firstArea = 0; //�퓬�J�n����

	void Init();
};
struct EnemyShotData
{
	Sphere sphere;//�����蔻��
	
	float area;
	bool exist;
	DirectX::XMFLOAT3 scale;
	float y;
	float moveTime;//enemy�{�̈ړ�����
	float time[2]; //0�p�x�ύX���� 1�V���b�g����
	float speed;
	void Init();
};
struct EnemyThunderData
{
	DirectX::XMFLOAT3 pos[THUNDERNUM];
	DirectX::XMFLOAT3 playerPos[THUNDERNUM];
	float w[THUNDERNUM];
	bool exist;
	Cylinder cylinder;//�����蔻��
	float area;
	float attackStartTime;//�����蔻��start
	float attackEndTime;//�����蔻��end
	float endTime;//state�I��
	void Init();
};
struct EnemyAttackData
{
	float angleTime;//�����ύX����
	float rushTime;
	float chargeTime;//���ߎ���
	bool rushFlag[3];
	void Init();
};
struct EnemyRAttackData
{
	//��
	DirectX::XMFLOAT3 effectScale1 = {0,0,0};
	DirectX::XMFLOAT3 effectScale2 = { 0,0,0 };
	float effectAngle1 = 0;
	float effectAngle2 = 0;

	DirectX::XMFLOAT3 defaultScale1;
	DirectX::XMFLOAT3 defaultScale2;
	float dispMaxTimer;
	float easingScale1 ;
	float easingScale2 ;
	float easingColor ;
	float effectColorW1;
	float effectColorW2 ;
	void Init();
};
struct EnemySpeedData
{
	float accelSpeed;
	float runSpeed;

	void Init();
};