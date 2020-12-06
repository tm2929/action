#pragma once
#pragma once
#include"GeometricPrimitive.h"
#include<vector>
#include<memory>
class HitAreaRnder
{
public:
	static void Create(ID3D11Device* device)
	{
		if (hitAreaRender != nullptr)return;
		hitAreaRender = new HitAreaRnder(device);
	}
	static HitAreaRnder& GetInctance()
	{
		return *hitAreaRender;
	}
	static void Destroy()
	{
		if (hitAreaRender == nullptr)return;
		delete(hitAreaRender);
		hitAreaRender = nullptr;
	}
	void SetHitCylinder(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max, const float& area,const DirectX::XMFLOAT4& color);
	void SetHitCube(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max);
	void SetHitSphere(const DirectX::XMFLOAT3& position, const float& area,const DirectX::XMFLOAT4& color);
	void SetHitSphere2(const DirectX::XMFLOAT3& position, const float& area, const DirectX::XMFLOAT4& color);
	void CalculateTransform(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);
	HitAreaRnder(ID3D11Device* device);
	void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4& light);
	void Clear();
private:
	static HitAreaRnder* hitAreaRender;
	std::unique_ptr<GeometricPrimitive>cylinderObj;
	std::unique_ptr<GeometricPrimitive>cubeObj;
	std::unique_ptr<GeometricPrimitive>sphereObj;
	struct Data
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1);
	};
	std::vector<Data>cylinderData;
	std::vector<Data>cubeData;
	std::vector<Data>sphereData;
};
#define pHitAreaRender (HitAreaRnder::GetInctance())