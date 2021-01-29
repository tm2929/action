#include "Character.h"

Character::Character(std::shared_ptr<ModelResource> resource)
	:Obj3D(resource)
{
	exist = true;
	Model* m = GetModel();
	m->PlayAnimation(animNo, true);

}

void Character::SetAnim(const int num, bool loop)
{
	animNo = num;
	Model* m = GetModel();
	m->PlayAnimation(animNo, loop);
}

const bool& Character::GetAnimContinue()
{
	Model* m = GetModel();
	return m->IsPlayAnimation();
}

void Character::AnimUpdate(float animtimer)
{
	Model* m = GetModel();
	m->UpdateAnimation(animtimer);
}

void Character::HitAreaTransform()
{
	DirectX::XMFLOAT3 position = GetPosition();
	hitArea.min = position;
	hitArea.min.y = position.y - 50;
	hitArea.max = hitArea.min;
	CalculateTransform();
	for (auto& boon : GetModel()->GetNodes())
	{
		if (hitArea.max.y < boon.worldTransform._42)
		{
			hitArea.max.y = boon.worldTransform._42;
		}
	}
}

void Character::HitAreaTransform(const float yLeng,const float area)
{
	hitArea.HitAreaTransform(yLeng,GetPosition(), area);
}

