#pragma once
#include "Obj3D.h"
#include "HitObjData.h"
class Character :public Obj3D
{
public:
	Character(std::shared_ptr<ModelResource>resource);

	void AnimUpdate(float animtimer);
	void HitAreaTransform();
	void HitAreaTransform(const float yLeng, const float area);
	//setter
	void SetAnim(const int num, bool loop = false);

	void SetHitFlag(const bool hit) { hitFlag = hit; }
	void SetHitPosition(const DirectX::XMFLOAT3& hitP) { hitPosition = hitP; }
	void SetHitAreaLeng(const float leng) { hitArea.area = leng; }
	void SetAttackFlag(const bool attack) { attackFlag = attack; }

	void SetFront(const DirectX::XMFLOAT3 front) { this->front = front; }
	void SetExist(const bool exist) { this->exist = exist; }
	void SetPower(const float power) { this->power = power; }
	void SetHp(const float hp) { this->hp = hp; }
	void SetSp(const float sp) { this->sp = sp; }
	void SetMaxHp(const float maxHp) { this->maxHp = maxHp; }
	void SetMaxSp(const float maxSp) { this->maxSp = maxSp; }
	
	void SetKnockBackFlag(const bool flag, int i = 0) { knockBackFlag = flag; knockBackType = i; }
	//getter
	const int& GetAnimNo()const { return animNo; }
	const bool& GetAnimContinue();

	const bool& GetHitFlag()const { return hitFlag; }
	const DirectX::XMFLOAT3& GetHitPosition()const { return hitPosition; }
	const Cylinder& GetHitArea()const { return hitArea; }
	const bool& GetAttackFlag()const { return attackFlag; }

	const DirectX::XMFLOAT3& GetFront() { return front; }
	const bool& GetExist()const { return exist; }
	const float& GetPower() { return power; }
	const float& GetHp() { return hp; }
	const float& GetSp() { return sp; }
	const float& GetMaxHp() { return maxHp; }
	const float& GetMaxSp() { return maxSp; }

	bool GetKnockBackFlag()const { return knockBackFlag; }
	int	GetKnockBackType()const { return knockBackType; }
private:
	int animNo = 0;
	Cylinder hitArea;
	DirectX::XMFLOAT3 hitPosition;
	bool hitFlag = false;
	bool attackFlag = false;
	bool exist = false;;
	float power = 0.f;;//çUåÇóÕ
	float hp = 0.f;
	float maxHp = 0.f;
	float sp = 0.f;
	float maxSp = 0.f;
	DirectX::XMFLOAT3 front = DirectX::XMFLOAT3(0, 0, 0);//ëOï˚

	bool knockBackFlag = false;
	int	knockBackType = 0;

};
