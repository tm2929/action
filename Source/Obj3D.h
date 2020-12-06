#pragma once
#pragma once
#include"Model.h"
#include"ModelResource.h"
#include<memory>
//3Dオブジェクトの基本情報管理クラス
class Obj3D
{
public:
	Obj3D(std::shared_ptr<ModelResource>resource);
	~Obj3D() {}
	//ワールド行列作成関数
	void CalculateTransform(bool angleFlag = true);//trueならXMMatrixRotationRollPitchYaw falseならXMMatrixRotationQuaternion

	int RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal);

	//setter
	void SetPosition(const DirectX::XMFLOAT3& p) { position = p; }
	void SetScale(const DirectX::XMFLOAT3& s) { scale = s; }
	void SetAngle(const DirectX::XMFLOAT3& a) { angle = a; }
	void SetOrientation(const DirectX::XMFLOAT4& a) { orientation = a; }
	void SetOrientations(const DirectX::XMFLOAT3& a);
	void SetWorld(const DirectX::XMFLOAT4X4& w) { world = w; }

	//getter
	const DirectX::XMFLOAT3& GetPosition()const { return position; }
	const DirectX::XMFLOAT3& GetScale()const { return scale; }
	const DirectX::XMFLOAT3& GetAngle()const { return angle; }
	const DirectX::XMFLOAT4X4& GetWorld()const { return world; }
	Model* GetModel() { return model.get(); }
	const DirectX::XMFLOAT3& GetBonePos(const char* bornName);
	const DirectX::XMFLOAT3& GetBoneDirectionX(const char* bornName);
	const DirectX::XMFLOAT3& GetBoneDirectionY(const char* bornName);
	const DirectX::XMFLOAT3& GetBoneDirectionZ(const char* bornName);
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT4 orientation;
	DirectX::XMFLOAT4X4 world;
	std::shared_ptr<Model>model;
};