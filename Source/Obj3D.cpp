#include "Obj3D.h"

Obj3D::Obj3D(std::shared_ptr<ModelResource> resource)
	:position(0, 0, 0), scale(1, 1, 1), angle(0, 0, 0)
{
	model = std::make_shared<Model>(resource);
}

void Obj3D::CalculateTransform(bool flag)
{
	DirectX::XMMATRIX W, s, r, t;
	//スケール行列
	s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転行列
	if (flag) r = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	else  r = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&orientation));
	//移動行列
	t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//ワールド行列
	W = s * r * t;

	model->CalculateLocalTransform();
	model->CalculateWorldTransform(W);

	DirectX::XMStoreFloat4x4(&world, W);
}


//レイピック
int Obj3D::RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition,
	DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal)
{
	DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&world);
	DirectX::XMMATRIX inverseTransform = DirectX::XMMatrixInverse(nullptr, worldTransform);

	// オブジェクト空間でのレイに変換
	DirectX::XMVECTOR worldStart = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMVECTOR worldEnd = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR localStart = DirectX::XMVector3TransformCoord(worldStart, inverseTransform);
	DirectX::XMVECTOR localEnd = DirectX::XMVector3TransformCoord(worldEnd, inverseTransform);

	// レイピック
	float outDistance;
	DirectX::XMFLOAT3 start, end;
	DirectX::XMStoreFloat3(&start, localStart);
	DirectX::XMStoreFloat3(&end, localEnd);
	int ret = model->RayPick(start, end, outPosition, outNormal, &outDistance);
	if (ret != -1)
	{
		// オブジェクト空間からワールド空間へ変換
		DirectX::XMVECTOR localPosition = DirectX::XMLoadFloat3(outPosition);
		DirectX::XMVECTOR localNormal = DirectX::XMLoadFloat3(outNormal);
		DirectX::XMVECTOR worldPosition = DirectX::XMVector3TransformCoord(localPosition, worldTransform);
		DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(localNormal, worldTransform);

		DirectX::XMStoreFloat3(outPosition, worldPosition);
		DirectX::XMStoreFloat3(outNormal, worldNormal);
	}

	return ret;
}

void Obj3D::SetOrientations(const DirectX::XMFLOAT3& direction)
{
	DirectX::XMFLOAT3 front = direction;

	DirectX::XMVECTOR fVec = DirectX::XMLoadFloat3(&front);
	DirectX::XMVECTOR frontVecN = DirectX::XMVector3Normalize(fVec);

	DirectX::XMFLOAT3 axis;
	FLOAT angle;
	DirectX::XMMATRIX mat;
	DirectX::XMVECTOR o;
	o = DirectX::XMLoadFloat4(&orientation);

	mat = DirectX::XMMatrixRotationQuaternion(o);
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, mat);


	DirectX::XMFLOAT3 forward = DirectX::XMFLOAT3(m._31, m._32, m._33);
	DirectX::XMVECTOR forwardVec = DirectX::XMLoadFloat3(&forward);

	DirectX::XMFLOAT3 up = DirectX::XMFLOAT3(m._21, m._22, m._23);
	DirectX::XMFLOAT3 right = DirectX::XMFLOAT3(m._11, m._12, m._13);

	DirectX::XMVECTOR c = DirectX::XMVector3Cross(forwardVec, frontVecN);
	DirectX::XMStoreFloat3(&axis, c);

	DirectX::XMVECTOR d = DirectX::XMVector3Dot(forwardVec, frontVecN);
	DirectX::XMStoreFloat(&angle, d);
	//axis = Vec3Cross(forward, fVecN);
//	angle = Vec3Dot(forward, front);

	angle = acosf(angle);


	//アングルが一定値よりも大きいとき
	if (0.001f < angle)
	{
		//axis = Vec3Normalize(axis);
		DirectX::XMVECTOR axisVec = DirectX::XMLoadFloat3(&axis);
		DirectX::XMVECTOR axisVecN = DirectX::XMVector3Normalize(axisVec);
		DirectX::XMVECTOR q;
		q = DirectX::XMQuaternionRotationAxis(axisVecN, angle);

		DirectX::XMVECTOR q2;
		q2 = DirectX::XMQuaternionMultiply(o, q);
		o = DirectX::XMQuaternionSlerp(o, q2, 0.1f);

	}
	DirectX::XMStoreFloat4(&orientation, o);

}

const DirectX::XMFLOAT3& Obj3D::GetBonePos(const char* bornName)
{
	DirectX::XMFLOAT3 pos;

	for (auto& boon : model->GetNodes())
	{
		if (strcmp(boon.name, bornName) == 0)
		{
			pos.x = boon.worldTransform._41;
			pos.y = boon.worldTransform._42;
			pos.z = boon.worldTransform._43;
		}
	}

	return pos;
}

const DirectX::XMFLOAT3& Obj3D::GetBoneDirectionX(const char* bornName)
{
	DirectX::XMFLOAT3 right;
	for (auto& boon : model->GetNodes())
	{
		if (strcmp(boon.name, bornName) == 0)
		{
			right.x = boon.worldTransform._21;
			right.y = boon.worldTransform._22;
			right.z = boon.worldTransform._23;
		}
	}
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&right);
	DirectX::XMVECTOR N = DirectX::XMVector3Length(vec);//正規化
	DirectX::XMStoreFloat3(&right, N);
	return right;
}

const DirectX::XMFLOAT3& Obj3D::GetBoneDirectionY(const char* bornName)
{
	DirectX::XMFLOAT3 up;
	for (auto& boon : model->GetNodes())
	{
		if (strcmp(boon.name, bornName) == 0)
		{
			up.x = boon.worldTransform._31;
			up.y = boon.worldTransform._32;
			up.z = boon.worldTransform._33;
		}
	}
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&up);
	DirectX::XMVECTOR N = DirectX::XMVector3Length(vec);//正規化
	DirectX::XMStoreFloat3(&up, N);
	return up;
}

const DirectX::XMFLOAT3& Obj3D::GetBoneDirectionZ(const char* bornName)
{
	DirectX::XMFLOAT3 flont;
	for (auto& boon : model->GetNodes())
	{
		if (strcmp(boon.name, bornName) == 0)
		{
			flont.x = -boon.worldTransform._11;
			flont.y = -boon.worldTransform._12;
			flont.z = -boon.worldTransform._13;
		}
	}
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&flont);
	DirectX::XMVECTOR N = DirectX::XMVector3Length(vec);//正規化
	DirectX::XMStoreFloat3(&flont, N);
	return flont;
}
