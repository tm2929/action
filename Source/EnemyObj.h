#pragma once
#pragma once
#include "Character.h"
class EnemyObj :public Character
{
public:
	EnemyObj(std::shared_ptr<ModelResource>resource);
	void BoneNameInput();
	void SaveBoneNum();
	void LoadBoneNum();
	void AttackBoneNameEditor();

	void  HitAttackTransform();
	//セッター
	void  SetHitAttackAreaLeng(const float leng) { hitSphere.area = leng; }
	void  SetThunderCylinder(const Cylinder cylinder) { thunderCylinder = cylinder; }
	void  SetDirection(const DirectX::XMFLOAT3 a) { direction = a; }
	void SetDamageFlag(const bool dam) { damageFlag = dam; }
	void SetRAttackFlag(const bool r) { rAttackFlag = r; }	
	void SetThunderAttackFlag(const bool t) { thunderAttackFlag = t; }
	void SetHighAttackFlag(const bool h) { highAttackFlag = h; }
	void SetRunSpeed(const float& runSpeed) { this->runSpeed = runSpeed; }
	void SetKnockBackSpeed(const float& knockBackSpeed) { this->knockBackSpeed = knockBackSpeed; }
	void SetKnockBackTime(const float& knockBackTime) { this->knockBackTime = knockBackTime; }
	void SetAccelSpeed(const float& accelSpeed) { this->accelSpeed = accelSpeed; }
	void SetHeadPosition()
	{
		Model::Node node = GetModel()->GetNodes()[12];

		headPosition.x = node.worldTransform._41;
		headPosition.y = node.worldTransform._42;
		headPosition.z = node.worldTransform._43;
	}
	//ゲッター
	const Sphere& GetHitSphere()const { return hitSphere; }
	const Cylinder& GetThunderCylinder()const { return thunderCylinder; }
	const DirectX::XMFLOAT3& GetDirection() { return  direction; }
	const bool& GetDamageFlag()const { return damageFlag; }
	const bool& GetRAttackFlag()const { return rAttackFlag; }
	const bool& GetThunderAttackFlag()const { return thunderAttackFlag; }
	const bool& GetHighAttackFlag()const { return highAttackFlag; }
	float GetRunSpeed()const { return runSpeed; }
	float GetKnockBackSpeed()const { return knockBackSpeed; }
	float GetKnockBackTime()const { return knockBackTime; }
	float GetAccelSpeed()const { return accelSpeed; }
	const DirectX::XMFLOAT3& GetHeadPosition() { return headPosition; }
private:
	Sphere hitSphere;//攻撃当たり判定
	Sphere shotSphere;//ショット
	Cylinder thunderCylinder;//雷
	DirectX::XMFLOAT3 headPosition;
	int attackBoneNum = 0;
	std::vector<std::string>boneName;
	DirectX::XMFLOAT3 pulsPos = { 32,-8,16 };
	DirectX::XMFLOAT3  direction;

	//attackline表示用
	bool attackLineFlag;
	//通常移動時
	float runSpeed = 0;

	//ノックバック時
	float  knockBackSpeed;
	float knockBackTime;

	bool damageFlag = false;
	//加速時
	float accelSpeed = 0;

	//回転攻撃かどうか
	bool rAttackFlag;
	//雷攻撃かどうか
	bool thunderAttackFlag;
	//強攻撃かどうか
	bool highAttackFlag;
};