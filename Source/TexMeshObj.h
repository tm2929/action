#pragma once
#pragma once
#pragma once

#include "TexMesh.h"
#include "Texture.h"

using namespace DirectX;
class TexMeshObj
{
public:
	TexMeshObj(/*bool useShadow = false*/) { texture = std::make_shared<Texture>(); }
	~TexMeshObj() {}

	enum class ObjectType
	{
		GeometricBorad,
		GeometricCube,
		GeometricSphere,


		MAX
	};

	void Create(ID3D11Device* device, const XMFLOAT3& scale, const XMFLOAT3& angle, const XMFLOAT3& position, const ObjectType& type, const wchar_t* filename);
	void Create(ID3D11Device* device, const XMFLOAT3& scale, const XMFLOAT3& angle, const XMFLOAT3& position, const ObjectType& type, std::shared_ptr<Texture>& texture);

	void Update()
	{
		DirectX::XMMATRIX S, R, T, W;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		W = S * R * T;
		DirectX::XMStoreFloat4x4(&world, W);
	}
	void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f))
	{
		if (obj && exist)
		{
			obj->SetCbMatrix(context, world, view, projection);
			obj->Render(context, view, projection, useShadow, color);
		}
	}

	int RayPick(const XMFLOAT3& startPosition, const XMFLOAT3& endPosition, XMFLOAT3* outPosition, XMFLOAT3* outNormal);

	void SetScale(const XMFLOAT3& scale) { this->scale = scale; };
	void SetAngle(const XMFLOAT3& angle) { this->angle = angle; }
	void SetPosition(const XMFLOAT3& position) { this->position = position; }
	void SetGeometric(std::shared_ptr<TexMesh>& obj) { this->obj = obj; }
	void SetTexture(std::shared_ptr<Texture>& texture) { this->texture = texture; }
	void SetTexture(ID3D11Device* device, const wchar_t* filename);
	void SetGeometric(ID3D11Device* device, const ObjectType& type);
	void SetExist(bool exist) { this->exist = exist; }
	//void SetUseShadowFlg(bool flg) { useShadow = flg; }

	const XMFLOAT3& GetScale() { return scale; }
	const XMFLOAT3& GetAngle() { return angle; }
	const XMFLOAT3& GetPosition() { return position; }
	const ObjectType& GetType() const { return type; }
	const bool GetExist() const { return exist; }
	Texture* GetTexture() { return texture.get(); }

private:
	std::shared_ptr<TexMesh> obj = nullptr;

	XMFLOAT3 scale = VECTOR3(1, 1, 1);
	XMFLOAT3 angle = VECTOR3(0, 0, 0);
	XMFLOAT3 position = VECTOR3(0, 0, 0);
	XMFLOAT4X4 world = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	ObjectType type;
	std::shared_ptr<Texture> texture = nullptr;

	bool exist = true;

	bool useShadow = false;
};