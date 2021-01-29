#pragma once
#include "Character.h"
class PlayerObj :public Character
{
public:
	//�q�b�g�X�g�b�v�p
	bool stopFlag = false;
	PlayerObj(std::shared_ptr<ModelResource>resource);
	void BoneNameInput();
	void SaveBoneNum();
	int LoadBoneNum();
	void SaveKickBoneNum();
	int LoadKickBoneNum();
	void AttackBoneNameEditor();

	void SetDamageFlag(const bool dam) { damageFlag = dam; }
	const Sphere& GetHitSphere()const { return hitSphere; }
	const bool& GetDamageFlag()const { return damageFlag; }
	//�Q�b�^�[
	const float& GetMaxMoveSpeed() { return maxMoveSpeed; }
	const float& GetAcceleration() { return acceleration; }
	const float& GetDeceleration() { return deceleration; }
	const float& GetTurnSpeed() { return turnSpeed; }
	const DirectX::XMFLOAT3& GetHeadPosition() { return headPosition; }
	const float& GetAccelTime() { return accelTime; }
	const float& GetAccelSpeed() { return accelSpeed; }
	const bool& GetAccelFlag() { return  accelFlag; }
	const bool& GetHitStateFlag() { return  hitStateFlag; }
	const float& GetEnmKnockBack() { return  enmKnockBack; }
	float GetKnockBackSpeed()const { return knockBackSpeed; }
	float GetKnockBackTime()const { return knockBackTime; }
	const bool& GetTrajectoryFlag() { return  trajectoryFlag; }

	//�Z�b�^�[
	void SetMaxMoveSpeed(const float maxMoveSpeed) { this->maxMoveSpeed = maxMoveSpeed; }
	void SetAcceleration(const float acceleration) { this->acceleration = acceleration; }
	void SetDeceleration(const float deceleration) { this->deceleration = deceleration; }
	void SetTurnSpeed(const float turnSpeed) { this->turnSpeed = turnSpeed; }
	void SetAccelTime(const float accelTime) { this->accelTime = accelTime; }
	void SetAccelSpeed(const float accelSpeed) { this->accelSpeed = accelSpeed; }
	void SetAccelFlag(const bool accelFlag) { this->accelFlag = accelFlag; }
	void SetHitStateFlag(const bool hitFlag) { this->hitStateFlag = hitFlag; }
	void SetEnmKnockBack(const float enmKnockBack) { this->enmKnockBack = enmKnockBack; }
	void SetKnockBackSpeed(const float knockBackSpeed) { this->knockBackSpeed = knockBackSpeed; }
	void SetKnockBackTime(const float knockBackTime) { this->knockBackTime = knockBackTime; }
	void SetTrajectoryFlag(const bool trajectoryFlag) { this->trajectoryFlag = trajectoryFlag; }
	void SetHeadPosition()
	{
		Model::Node node = GetModel()->GetNodes()[12];

		headPosition.x = node.worldTransform._41;
		headPosition.y = node.worldTransform._42;
		headPosition.z = node.worldTransform._43;
	}
	//�����蔻��
	void  HitAttackTransform();
	void  HitAttackTransformIMGUI(bool kickFlag=false);
	void  SetHitAttackAreaLeng(const float leng) { hitSphere.area = leng; }
private:
	Sphere hitSphere;//�U�������蔻��
	int attackBoneNum = 0;
	int kickBoneNum = 0;
	std::vector<std::string>boneName;
	DirectX::XMFLOAT3 pulsPos = { 110,100,110 };
	DirectX::XMFLOAT3 headPosition;
	//�ʏ�ړ���
	float maxMoveSpeed = 0;//MaxSpeed
	float acceleration = 0;//����
	float deceleration = 0;//����
	float turnSpeed = DirectX::XMConvertToRadians(360);

	bool damageFlag = false;
	//�����_�b�V����
	float accelTime = 0;
	float accelSpeed = 0;
	bool  accelFlag = false;
	DirectX::XMINT3 tstPos = { 0,0,0 };
	float enmKnockBack = 0;
	//�O�Օ\���t���O
	bool trajectoryFlag = false;

	bool hitStateFlag; //state��hit���ǂ���
	//�m�b�N�o�b�N��
	float knockBackSpeed = 0;
	float knockBackTime;

};