#include "HitAreaRnder.h"
HitAreaRnder* HitAreaRnder::hitAreaRender = nullptr;

void HitAreaRnder::SetHitCylinder(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max, const float& area, const DirectX::XMFLOAT4& color)
{
	float yLeng = (max.y - min.y);
	Data d;
	d.position = min;
	d.position.y += yLeng * 0.5f;
	d.scale = DirectX::XMFLOAT3(area * 2.f, yLeng, area * 2.f);
	d.color = color;
	cylinderData.push_back(d);
}
void HitAreaRnder::SetHitCube(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max, const DirectX::XMFLOAT4& color)
{
	float xLeng = (max.x - min.x) * 0.5f;
	float yLeng = (max.y - min.y) * 0.5f;
	float zLeng = (max.z - min.z) * 0.5f;
	Data d;
	d.position = DirectX::XMFLOAT3(min.x + xLeng, min.y + yLeng, min.z + zLeng);
	d.scale = DirectX::XMFLOAT3(xLeng*2.f, yLeng * 2.f, zLeng * 2.f);
	d.color = color;
	cubeData.push_back(d);
}
void HitAreaRnder::SetHitSphere(const DirectX::XMFLOAT3& position, const float& area, const DirectX::XMFLOAT4 &color)
{
	Data d;
	d.position = position;
	d.scale = DirectX::XMFLOAT3(area * 2.f, area * 2.f, area * 2.f);
	d.color = color;
	sphereData.push_back(d);

}
void HitAreaRnder::SetHitSphere2(const DirectX::XMFLOAT3& position, const float& area, const DirectX::XMFLOAT4& color)
{
	Data d;
	d.position = position;
	d.scale = DirectX::XMFLOAT3(area * 2.f, area, area * 2.f);
	d.color = color;
	sphereData.push_back(d);

}
void HitAreaRnder::CalculateTransform(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	DirectX::XMMATRIX V, P;
	V = DirectX::XMLoadFloat4x4(&view);
	P = DirectX::XMLoadFloat4x4(&projection);
	//円柱
	for (auto& d : cylinderData)
	{
		DirectX::XMMATRIX W, s, r, t;
		//スケール行列
		s = DirectX::XMMatrixScaling(d.scale.x, d.scale.y, -d.scale.z);
		//回転行列
		r = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);
		//移動行列
		t = DirectX::XMMatrixTranslation(d.position.x, d.position.y, d.position.z);
		//ワールド行列
		W = s * r * t;
		DirectX::XMStoreFloat4x4(&d.world, W);

		DirectX::XMStoreFloat4x4(&d.worldViewProjection, W * V * P);
	}
	//キューブ
	for (auto& d : cubeData)
	{
		DirectX::XMMATRIX W, s, r, t;
		//スケール行列
		s = DirectX::XMMatrixScaling(d.scale.x, d.scale.y, d.scale.z);
		//回転行列
		r = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);
		//移動行列
		t = DirectX::XMMatrixTranslation(d.position.x, d.position.y, d.position.z);
		//ワールド行列
		W = s * r * t;
		DirectX::XMStoreFloat4x4(&d.world, W);

		DirectX::XMStoreFloat4x4(&d.worldViewProjection, W * V * P);
	}
	//円
	for (auto& d : sphereData)
	{
		DirectX::XMMATRIX W, s, r, t;
		//スケール行列
		s = DirectX::XMMatrixScaling(d.scale.x, d.scale.y, d.scale.z);
		//回転行列
		r = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);
		//移動行列
		t = DirectX::XMMatrixTranslation(d.position.x, d.position.y, d.position.z);
		//ワールド行列
		W = s * r * t;
		DirectX::XMStoreFloat4x4(&d.world, W);

		DirectX::XMStoreFloat4x4(&d.worldViewProjection, W * V * P);
	}

}

HitAreaRnder::HitAreaRnder(ID3D11Device* device)
{
	cylinderObj = std::make_unique<GeometricCylinder>(device, 32);
	sphereObj = std::make_unique<GeometricSphere>(device, 32, 16);
	cubeObj = std::make_unique<GeometricCube>(device);
}

void HitAreaRnder::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4& light)
{
	for (auto& d : cylinderData)
	{
		cylinderObj->render(context, DirectX::XMFLOAT4(d.color), light, d.worldViewProjection, d.world);
	}
	for (auto& d : sphereData)
	{
		sphereObj->render(context, DirectX::XMFLOAT4(d.color), light, d.worldViewProjection, d.world);
	}
	for (auto& d : cubeData)
	{
		cubeObj->render(context, DirectX::XMFLOAT4(d.color), light, d.worldViewProjection, d.world,true);
	}
}

void HitAreaRnder::Clear()
{
	cylinderData.clear();
	sphereData.clear();
	cubeData.clear();
}
