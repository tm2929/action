#include "Weapon.h"

Weapon::Weapon(std::shared_ptr<ModelResource> resource)
	:Character(resource)
{
	SetScale(DirectX::XMFLOAT3(1, 1, 1));
	SetPosition(DirectX::XMFLOAT3(0, 0, 0));
	SetAngle(DirectX::XMFLOAT3(0, 0, 0));
	CalculateTransform();
}

void Weapon::Update(float elapsedTime, std::shared_ptr<Obj3D>  obj)
{
	Model::Node* attackNode = &obj->GetModel()->GetNodes()[weaponPos];
	//�r�̃��[���h��ԍs��𕐊�̃��[�J����ԍs��ɂ����邱�ƂŁA�r�ɂ��������B
	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&attackNode->worldTransform);
	
	GetModel()->CalculateLocalTransform();
	GetModel()->CalculateWorldTransform(WorldTransform);
}
