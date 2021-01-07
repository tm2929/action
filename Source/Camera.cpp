#include "camera.h"
#include "gamepad.h"
#include "KeyInput.h"
#include "HitAreaRnder.h"
Camera* Camera::camera = nullptr;
DirectX::XMFLOAT2 Camera::viewPort;
Camera::Camera(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	this->eye = eye;
	this->focus = focus;
	this->up = up;
	type = Camera::TYPE::TARGET;
	targetPos = DirectX::XMFLOAT3(0, 0, 0);
	enemyPos = DirectX::XMFLOAT3(0, 0, 0);
	len = DirectX::XMFLOAT3(0, 0, 0);
	angle = DirectX::XMFLOAT3(0, DirectX::XMConvertToRadians(0.0f), 0);

	FoVattenRate = 3.0f; //FoVの減衰比率
	attackFoV[0] = 48.0f;
	attackFoV[1] = 46.0f;
	attackFoV[2] = 44.0f;
	runFoV = 52;
	waiteFoV = 50.0f;

	aspect = viewPort.x / viewPort.y;//画面比率
	nearZ = 0.1f;//表示最近面までの距離
	farZ = 10000.0f;//表紙最遠面までの距離

	FoV = waiteFoV;

	cubePos = { 0,30,0 };
	cubeMax = { 5,5,5 };
	cubeMin = { 5,5,5 };

}

Camera::Camera() : eye(0, 0, -200.0f), focus(0, 0, 0), up(0, 1, 0), front(0, 0, 1), right(1, 0, 0), view(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0), FoV(0), aspect(0), nearZ(0), farZ(0)
{

}

void Camera::SetCamera(const DirectX::XMFLOAT3& leng, const TYPE& type)
{
	this->len = leng;
	this->type = static_cast<int>(type);
	up = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
}

void Camera::CalculateTransforms()
{

	//ビュー行列
	DirectX::XMMATRIX V, P;

	//カメラの設定
	DirectX::XMVECTOR e, f, u;
	e = DirectX::XMVectorSet(eye.x, eye.y, eye.z, 1.0f);//視点
	f = DirectX::XMVectorSet(focus.x, focus.y, focus.z, 1.0f);//注視点
	u = DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f);//上ベクトル

	V = DirectX::XMMatrixLookAtLH(e, f, u);
	DirectX::XMStoreFloat4x4(&view, V);


	//角度をラジアン(θ)に変換
	float fov_y = FoV * (3.14f / 180.f);//角度

	P = flag ? DirectX::XMMatrixPerspectiveFovLH(fov_y, aspect, nearZ, farZ) : DirectX::XMMatrixOrthographicLH(FoV, FoV / aspect, 1.0f, farZ);
	DirectX::XMStoreFloat4x4(&projection, P);
	//前方向
	front.x = view._31;
	front.y = view._32;
	front.z = view._33;
	//横方向
	right.x = view._11;
	right.y = view._12;
	right.z = view._13;

}

void Camera::Updata(float elapsedTime)
{
	float x = focus.x - eye.x;
	float y = focus.y - eye.y;
	float z = focus.z - eye.z;
	distance = ::sqrtf(x * x + y * y + z * z);
	switch (type)
	{
	case TYPE::RELATIVE_POS:
		RelativePosCamera();
		break;
	case TYPE::TARGET:
		TargetCamera(elapsedTime);
		break;
	case TYPE::FREE:
		FreeCamera();
		break;
	case TYPE::CLEAR:
		ClearCamera(elapsedTime);
		break;
	case TYPE::OVER:
		OverCamera(elapsedTime);
		break;
	}
	Vibrate(elapsedTime);

	cubePos = eye;

	cube.max.x = cubePos.x + cubeMax.x;
	cube.max.y = cubePos.y + cubeMax.y;
	cube.max.z = cubePos.z + cubeMax.z;

	cube.min.x = cubePos.x - cubeMin.x;
	cube.min.y = cubePos.y - cubeMin.y;
	cube.min.z = cubePos.z - cubeMin.z;

	pHitAreaRender.SetHitCube(cube.min, cube.max, DirectX::XMFLOAT4(1, 1, 1, 1));
}

void Camera::RelativePosCamera()
{
	float power_x = .0f;
	float power_y = .0f;

	power_x = input::StickStateX(0, true);
	power_y = input::StickStateY(0, true);

	if (KeyInput::KeyState() & KEY_LEFT) power_x = -1.0f;
	if (KeyInput::KeyState() & KEY_RIGHT)power_x = 1.0f;
	if (KeyInput::KeyState() & KEY_UP)   power_y = -1.0f;
	if (KeyInput::KeyState() & KEY_DOWN) power_y = 1.0f;

	static float Pangle = DirectX::XMConvertToRadians(3.5f);

	if (power_x < -0.3)
	{
		angle.y -= Pangle;
	}
	else if (power_x > 0.3)
	{
		angle.y += Pangle;
	}

	if (power_y < -0.3)
	{
		angle.x -= Pangle * 10;
	}
	else if (power_y > 0.3)
	{
		angle.x += Pangle * 10;
	}
	if (angle.x > 5)
	{
		angle.x = 5;
	}
	else if (angle.x < -5)
	{
		angle.x = -5;
	}

	if (angle.y > DirectX::XMConvertToRadians(360))
	{
		angle.y = 0;
	}
	if (angle.y < DirectX::XMConvertToRadians(-360))
	{
		angle.y = 0;
	}
	//DirectX::XMFLOAT4X4 m;
	//DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z));
	//eye = DirectX::XMFLOAT3(targetPos.x + m._11 * leng.x, targetPos.y + m._21  * leng.y, targetPos.z + m._31 * leng.z);
	eye = DirectX::XMFLOAT3(targetPos.x + sinf(angle.y) * len.x, targetPos.y + angle.x + len.y, targetPos.z + cosf(angle.y) * len.z);
	if (lerpFlag)
	{
		//補完
		DirectX::XMVECTOR f = DirectX::XMLoadFloat3(&focus);
		DirectX::XMVECTOR t = DirectX::XMLoadFloat3(&targetPos);

		f = DirectX::XMVectorLerp(f, t, 0.4f);
		DirectX::XMFLOAT3 ff;
		DirectX::XMStoreFloat3(&ff, f);

		focus = ff;
		static int time = 0;
		if (time++ > 4)
		{
			lerpFlag = false;
			time = 0;
		}
	}
	else
	{
		focus = targetPos;
	}
}

void Camera::TargetCamera(float elapsed_time)
{
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&targetPos);
	//DirectX::XMFLOAT3 enmPos = { m_enemy_pos.x,m_enemy_pos.y,-m_enemy_pos.z };
	DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&enemyPos);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(e, p);
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);//正規化

	DirectX::XMVECTOR length_v = DirectX::XMVectorScale(dir, -len.x);

	DirectX::XMFLOAT3 leng;

	DirectX::XMStoreFloat3(&leng, length_v);

	eye = DirectX::XMFLOAT3(targetPos.x + leng.x, targetPos.y + len.y, targetPos.z + leng.z);

	DirectX::XMFLOAT3 n = DirectX::XMFLOAT3(-sinf(0), 0, -cosf(0));
	DirectX::XMVECTOR vn = DirectX::XMLoadFloat3(&n);

	DirectX::XMVECTOR d = DirectX::XMVector3Dot(vn, dir);//内積ででるのはcosθ -180～180
	float dot;
	DirectX::XMStoreFloat(&dot, d);
	DirectX::XMVECTOR c = DirectX::XMVector3Cross(vn, dir);
	DirectX::XMFLOAT3 cross;
	DirectX::XMStoreFloat3(&cross, c);
	if (cross.y < 0)
	{
		angle.y = acosf(-dot) + 3.14f;;//acosfでシータ（角度）がでる　
	}
	else
	{
		angle.y = acosf(dot);
	}
	//補完
	DirectX::XMVECTOR f = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR target = DirectX::XMLoadFloat3(&enemyPos);
	f = DirectX::XMVectorLerp(f, target, 0.1f);
	DirectX::XMFLOAT3 ff;
	DirectX::XMStoreFloat3(&ff, f);

	focus = ff;
}
void Camera::FreeCamera()
{
	POINT cursor;
	::GetCursorPos(&cursor);

	oldCursor = newCursor;
	newCursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

	float move_x = (newCursor.x - oldCursor.x) * 0.02f;
	float move_y = (newCursor.y - oldCursor.y) * 0.02f;

	// Altキー
	if (::GetAsyncKeyState(VK_MENU) & 0x8000)
	{
		if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			// Y軸回転
			rotateY += move_x * 0.5f;
			if (rotateY > DirectX::XM_PI)
			{
				rotateY -= DirectX::XM_2PI;
			}
			else if (rotateY < -DirectX::XM_PI)
			{
				rotateY += DirectX::XM_2PI;
			}
			// X軸回転
			rotateX += move_y * 0.5f;
			if (rotateX > DirectX::XM_PI)
			{
				rotateX -= DirectX::XM_2PI;
			}
			else if (rotateX < -DirectX::XM_PI)
			{
				rotateX += DirectX::XM_2PI;
			}
		}
		else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
		{
			// 平行移動
			float s = distance * 0.035f;
			float x = -move_x * s;
			float y = move_y * s;

			focus.x += right.x * x;
			focus.y += right.y * x;
			focus.z += right.z * x;

			focus.x += up.x * y;
			focus.y += up.y * y;
			focus.z += up.z * y;
		}
		else if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			// ズーム
			distance += (-move_y - move_x) * distance * 0.1f;
		}
		float sx = ::sinf(rotateX);
		float cx = ::cosf(rotateX);
		float sy = ::sinf(rotateY);
		float cy = ::cosf(rotateY);

		DirectX::XMVECTOR front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
		DirectX::XMVECTOR right = DirectX::XMVectorSet(cy, 0, -sy, 0.0f);
		DirectX::XMVECTOR up = DirectX::XMVector3Cross(right, front);

		DirectX::XMVECTOR focus = DirectX::XMLoadFloat3(&this->focus);
		DirectX::XMVECTOR distance = DirectX::XMVectorSet(this->distance, this->distance, this->distance, 0.0f);
		DirectX::XMVECTOR eye = DirectX::XMVectorSubtract(focus, DirectX::XMVectorMultiply(front, distance));
		//focus = DirectX::XMVectorSubtract(focus, DirectX::XMVectorMultiply(front, distance));

		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(eye, focus, up);
		view = DirectX::XMMatrixTranspose(view);

		right = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1, 0, 0, 0), view);
		up = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 1, 0, 0), view);
		front = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), view);

		DirectX::XMStoreFloat3(&this->eye, eye);
		//DirectX::XMStoreFloat3(&m_focus, focus);
		DirectX::XMStoreFloat3(&this->up, up);
		DirectX::XMStoreFloat3(&this->right, right);
	}
}
void Camera::Vibrate(float elapsedTime)
{
	int x = 0;
	int z = 0;
	DirectX::XMFLOAT3 focus = GetFocus();
	if (time > 0)
	{
		time -= elapsedTime;

		x = (rand() % 100 - 50);
		z = (rand() % 100 - 50);

		focus.x += x * range;
		focus.z += z * range;

		SetFocus(focus);

	}
}
void Camera::ClearCamera(float elapsedTime)
{
	DirectX::XMFLOAT3 length = { 85,40,85 };
	//補完
	DirectX::XMVECTOR lengthV;
	DirectX::XMVECTOR endLength = DirectX::XMLoadFloat3(&length);
	DirectX::XMVECTOR startLength = DirectX::XMLoadFloat3(&len);
	lengthV = DirectX::XMVectorLerp(startLength, endLength, 0.8f);
	DirectX::XMFLOAT3 ll;
	DirectX::XMStoreFloat3(&ll, lengthV);
	len = ll;
	eye = DirectX::XMFLOAT3(targetPos.x + sinf(angle.y) * len.x, targetPos.y + angle.x + len.y, targetPos.z + cosf(angle.y) * len.z);
	//eye = DirectX::XMFLOAT3(0,0,0);

	DirectX::XMVECTOR f = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR target = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(enemyPos.x, enemyPos.y + 10, enemyPos.z));
	f = DirectX::XMVectorLerp(f, target, 0.1f);
	DirectX::XMFLOAT3 ff;
	DirectX::XMStoreFloat3(&ff, f);

	focus = ff;

}
void Camera::OverCamera(float elapsedTime)
{
	DirectX::XMFLOAT3 length = { 55, -8, 55 };

}
void Camera::SetVibration(float range, float timer)
{
	this->range = range;
	this->time = timer;
}
void Camera::SetTarget(const DirectX::XMFLOAT3& target_pos)
{
	targetPos.x = target_pos.x;
	targetPos.y = target_pos.y;
	targetPos.z = target_pos.z;

}

void Camera::SetTargetAngle(const DirectX::XMFLOAT3& target_angle)
{
	targetAngle = target_angle;
}

void Camera::SetEnemy(const DirectX::XMFLOAT3& enemy_pos)
{
	enemyPos = enemy_pos;
}

