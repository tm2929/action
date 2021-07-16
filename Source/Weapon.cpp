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
	//腕のワールド空間行列を武器のローカル空間行列にかけることで、腕にもたせれる。
	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&attackNode->worldTransform);
	
	GetModel()->CalculateLocalTransform();
	GetModel()->CalculateWorldTransform(WorldTransform);
}
