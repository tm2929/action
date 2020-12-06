#include "Collision.h"
#include "Camera.h"


bool Collision::isHitAABB(const AABB& obj1, const AABB& obj2)
{
	return false;
}

bool Collision::isHitCylinder(const Cylinder& obj1, const Cylinder& obj2)
{
	if (obj1.max.y < obj2.min.y && obj1.min.y < obj2.max.y)return false;


	float leng = (obj1.min.x - obj2.min.x) * (obj1.min.x - obj2.min.x) + (obj1.min.z - obj2.min.z) * (obj1.min.z - obj2.min.z);
	if ((obj1.area + obj2.area) * (obj1.area + obj2.area) < leng)return false;

	return true;
}

bool Collision::isHitSphere(const Sphere& obj1, const Sphere& obj2)
{
	float leng = (obj1.position.x - obj2.position.x) * (obj1.position.x - obj2.position.x) + (obj1.position.y - obj2.position.y) * (obj1.position.y - obj2.position.y) + (obj1.position.z - obj2.position.z) * (obj1.position.z - obj2.position.z);
	if ((obj1.area + obj2.area) * (obj1.area + obj2.area) >= leng)
	{
		return true;
	}
	return false;
}

bool Collision::isHitCylinderSphere(const Cylinder& cylinder, const Sphere& sphere)
{
	DirectX::XMVECTOR length, pos1, pos2, vec;
	DirectX::XMFLOAT3 position;
	if (cylinder.max.y < sphere.position.y)
	{
		position = cylinder.max;
	}
	else if (cylinder.min.y > sphere.position.y)
	{
		position = cylinder.min;
	}
	else
	{
		DirectX::XMVECTOR p0, p1, p2, p3, vec1, vec2, dot;
		p0 = DirectX::XMLoadFloat3(&cylinder.max);
		p1 = DirectX::XMLoadFloat3(&cylinder.min);
		p2 = DirectX::XMLoadFloat3(&sphere.position);

		vec1 = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(p1, p0));
		vec2 = DirectX::XMVectorSubtract(p2, p0);

		dot = DirectX::XMVector3Dot(vec1, vec2);

		p3 = DirectX::XMVectorAdd(p0, DirectX::XMVectorMultiply(vec1, dot));
		DirectX::XMStoreFloat3(&position, p3);
	}
	pos1 = DirectX::XMLoadFloat3(&sphere.position);
	pos2 = DirectX::XMLoadFloat3(&position);
	vec = DirectX::XMVectorSubtract(pos2, pos1);
	length = DirectX::XMVector3Length(vec);
	float fLength;
	DirectX::XMStoreFloat(&fLength, length);
	if (fLength > sphere.area + cylinder.area)return false;
	return true;
}

bool Collision::HitBoxCheak(const DirectX::XMFLOAT2& min1, const DirectX::XMFLOAT2& max1, const DirectX::XMFLOAT2& min2, const DirectX::XMFLOAT2& max2)
{
	float right1 = max1.x;	float right2 = max2.x;
	float left1 = min1.x;	float left2 = min2.x;
	float top1 = max1.y;	float top2 = max2.y;
	float bottom1 = min1.y;	float bottom2 = min2.y;

	if (right1 < left2)	return false;
	if (left1 > right2)	return false;
	if (top1 < bottom2)	return false;
	if (bottom1 > top2)		return false;

	return true;
}
//min(左下手前)、max(右上奥)
bool Collision::HitCubeCheak(const DirectX::XMFLOAT3& min1, const DirectX::XMFLOAT3& max1, const DirectX::XMFLOAT3& min2, const DirectX::XMFLOAT3& max2)
{
	float right1 = max1.x;	float right2 = max2.x;
	float left1 = min1.x;	float left2 = min2.x;
	float top1 = max1.y;	float top2 = max2.y;
	float bottom1 = min1.y;	float bottom2 = min2.y;
	float back1 = max1.z;	float back2 = max2.z;
	float front1 = min1.z;	float front2 = min2.z;

	if (right1 < left2)	return false;
	if (left1 > right2)	return false;
	if (top1 < bottom2)	return false;
	if (bottom1 > top2)		return false;
	if (back1 < front2)	return false;
	if (front1 > back2)	return false;

	return true;
}

void Collision::WorldToScreen(DirectX::XMFLOAT3* screenPosiotion, const DirectX::XMFLOAT3& worldPosition, float screenWidth, float screenHeight)
{

	Camera& camera = Camera::GetInstance();
	float viewportX = 0.f;
	float viewportY = 0.f;
	float viewportW = screenWidth;
	float viewportH = screenHeight;
	float viewportMinZ = 0.f;
	float viewportMaxZ = 1.f;

	// ビュー行列
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&camera.GetView());
	// プロジェクション行列
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&camera.GetProjection());
	// ワールド座標
	DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();

	// ワールド座標からNDC座標へ変換
	DirectX::XMMATRIX WVP = W * V * P;
	DirectX::XMVECTOR NDCPosition = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&worldPosition), WVP);
	DirectX::XMFLOAT3 ndcPosition;
	DirectX::XMStoreFloat3(&ndcPosition, NDCPosition);
	// NDC座標からスクリーン座標へ変換
	//screenPosiotion->x = (ndcPosition.x + 1.f) * viewportW * 0.5f;
	//screenPosiotion->y = (1.f - viewportH) * -ndcPosition.y * 0.5f;
	//screenPosiotion->z = viewportMinZ + ndcPosition.z * (viewportMaxZ - viewportMinZ);
	screenPosiotion->x = viewportX + (1.0f + ndcPosition.x) * viewportW * 0.5f;
	screenPosiotion->y = viewportY + (1.0f - ndcPosition.y) * viewportH * 0.5f;
	screenPosiotion->z = viewportMinZ + ndcPosition.z * (viewportMaxZ - viewportMinZ);
}

void Collision::ScreenToWorld(DirectX::XMFLOAT3* worldPosition, const DirectX::XMFLOAT3& screenPosiotion, float screenWidth, float screenHeight)
{
	Camera& camera = Camera::GetInstance();

	float viewportX = 0.f;
	float viewportY = 0.f;
	float viewportW = screenWidth;
	float viewportH = screenHeight;
	float viewportMinZ = 0.f;
	float viewportMaxZ = 1.f;

	// ビュー行列
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&camera.GetView());
	// プロジェクション行列
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&camera.GetProjection());
	// ワールド座標
	DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();

	// スクリーン座標から NDC 座標へ変換
	DirectX::XMVECTOR WPos = DirectX::XMVector3Unproject(DirectX::XMLoadFloat3(&screenPosiotion),
		viewportX, viewportY, viewportW, viewportH,
		viewportMinZ, viewportMaxZ,
		P, V, W);
	DirectX::XMStoreFloat3(worldPosition, WPos);
}

int Collision::MoveCheck(Obj3D& obj, const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition)
{
	//レイピック　
	DirectX::XMFLOAT3  hitPosition, hitNormal;
	int materoalIndex = obj.RayPick(startPosition, endPosition, &hitPosition, &hitNormal);
	if (materoalIndex == -1)
	{
		//ヒットしなかったら移動後の位置は終点
		*outPosition = endPosition;
		return materoalIndex;
	}
	//壁を突き抜けたベクトル
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);

	// 壁の法線ベクトルを単位化
	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal));

	// 入射ベクトルを法線に射影
	DirectX::XMVECTOR dot = DirectX::XMVectorNegate(DirectX::XMVector3Dot(vec, normal));

	//補正位置の計算
	DirectX::XMVECTOR dot2 = DirectX::XMVectorMultiply(normal, dot);
	DirectX::XMVECTOR position = DirectX::XMVectorAdd(end, dot2);

	DirectX::XMStoreFloat3(outPosition, position);

	//補正後の位置が壁にめり込んでいた場合は移動しないようにする

	DirectX::XMFLOAT3  hithitpos;
	materoalIndex = obj.RayPick(startPosition, *outPosition, &hithitpos, &hitNormal);
	if (materoalIndex != -1)
	{
		*outPosition = startPosition;
		return materoalIndex;
	}

	return materoalIndex;
}

