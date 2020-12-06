#pragma once
#include"HitObjData.h"
#include "Obj3D.h"
class Collision
{

public:
	static bool isHitAABB(const AABB& obj1, const AABB& obj2);
	static bool isHitCylinder(const Cylinder& obj1, const Cylinder& obj2);
	static bool isHitSphere(const Sphere& obj1, const Sphere& obj2);
	static bool isHitCylinderSphere(const Cylinder& cylinder, const Sphere& sphere);
	bool HitBoxCheak(const DirectX::XMFLOAT2& min1, const DirectX::XMFLOAT2& max1, const DirectX::XMFLOAT2& min2, const DirectX::XMFLOAT2& max2);
	bool HitCubeCheak(const DirectX::XMFLOAT3& min1, const DirectX::XMFLOAT3& max1, const DirectX::XMFLOAT3& min2, const DirectX::XMFLOAT3& max2);

	// ワールド座標(3D)からスクリーン座標(2D)へ変換
	static void WorldToScreen(DirectX::XMFLOAT3* screenPosiotion, const DirectX::XMFLOAT3& worldPosition, float screenWidth, float screenHeight);
	// スクリーン座標(2D)からワールド座標(3D)へ変換
	static void ScreenToWorld(DirectX::XMFLOAT3* worldPosition, const DirectX::XMFLOAT3& screenPosiotion, float screenWidth, float screenHeight);

	static int RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal);

	static int MoveCheck(Obj3D& obj, const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition);


};