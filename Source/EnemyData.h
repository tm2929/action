#pragma once
#include <DirectXMath.h>
#include "HitObjData.h"
#define THUNDERNUM 1
struct EnemyHitData
{
	float damage;//��b�_���[�W
	float maxDamageMagnification;
	float attackPower[3];//�A�����Z
	DirectX::XMFLOAT3 pulsPos;//�U�����ړ�����
	float pulsDamage;//�A���U�������Z�̓��ꕨ
	float hitAttackLength;//�U�������蔻��̒���
	float hitLength;//�{�̓����蔻��̒���
	float attackTime;//�U�����[�V�����؂�ւ�����
	int count = 0;//�U���J�n�t���[��
	void Init();
};
struct EnemyChoiceData
{
	float waiteTime = 0;//�ҋ@����
	float time = 0;
	int num;//debug���A�U��number�ύX�p
	float choiceLen[3];	//�U���I������
	bool firstFlag; //��ԍŏ��̓v���C���[��������x�߂Â��Ă���{�X����͂��߂�悤��
	float firstArea; //�퓬�J�n����

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
struct EnemySpeedData
{
	float accelSpeed;
	float runSpeed;

	void Init();
};