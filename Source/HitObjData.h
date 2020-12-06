#pragma once
#include<DirectXMath.h>
struct Cylinder
{
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;
	float area;
	void HitAreaTransform(const float yLeng, DirectX::XMFLOAT3 pos, float area)
	{
		min = pos;
		max = min;
		max.y = yLeng;
		this->area = area;
	}
};

struct AABB
{
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;
};
struct Sphere
{
	DirectX::XMFLOAT3 position;
	float area;
	void HitAreaTransform(DirectX::XMFLOAT3 pos, float area)
	{
		position = pos;
		this->area = area;
	}
};

