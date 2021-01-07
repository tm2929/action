#pragma once
#include<DirectXMath.h>
#include <d3d11.h>
#include "HitObjData.h"
class Camera
{
private:

	bool lerpFlag = false;
	bool targetFlag = false;
	bool freeFlag = false;
	static Camera* camera;
	float time = 0.0f;  //�U������
	float range = 0.0f; //�U����
	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 focus;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 front = { 0, 0, 1 };
	DirectX::XMFLOAT3 right = { 1, 0, 0 };

	DirectX::XMFLOAT3 targetPos;
	DirectX::XMFLOAT3 targetAngle;
	DirectX::XMFLOAT3 enemyPos;

	DirectX::XMFLOAT3 len;
	static DirectX::XMFLOAT2 viewPort;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	Camera(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);
	int type;

	DirectX::XMFLOAT2	oldCursor;
	DirectX::XMFLOAT2	newCursor;
	float				rotateX = 0.0f;
	float				rotateY = 0.0f;
	float				distance = 0.0f;
	//�����蔻��
	AABB cube;
	DirectX::XMFLOAT3 cubeMax = { 0,0,0 };
	DirectX::XMFLOAT3 cubeMin = { 0,0,0 };
	DirectX::XMFLOAT3 cubePos = { 0,0,0 };

public:
	DirectX::XMFLOAT3 angle;
	//FoV�ϓ�
	bool   flag = false;
	float  FoVattenRate = 0.f; //FoV�̌����䗦
	float  attackFoV[3];//�v���C���[�U����Fov
	float  runFoV;//�v���C���[�ړ���Fov
	float  waiteFoV = 0.0f;//�v���C���[��~��Fov
	float  FoV = 0.0f;

	float aspect;//�A�X�y�N�g��
	float nearZ; // �\���ŋߖʂ܂ł̋���
	float farZ;//�\���ŉ��ʂ܂ł̋���
	enum TYPE
	{
		RELATIVE_POS,//�ʏ�v���C���[�J�������E�ړ�����
		TARGET,//�G�l�~�[���b�N�I��
		FREE,//�t���[�J����
		CLEAR,//�N���A���J����
		OVER,//�I�[�o�[���J����
	};
	Camera();
	static void Create(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
	{
		if (camera != nullptr)
			return;

		camera = new Camera(eye, focus, up);

	}
	void SetPerspective(float fov, float aspect, float nearZ, float farZ)
	{
		FoV = fov;
		this->aspect = aspect;
		this->nearZ = nearZ;
		this->farZ = farZ;
	}
	void SetCamera(const DirectX::XMFLOAT3& leng, const TYPE& type);//�J�����ύX
	void CalculateTransforms();
	void Updata(float elapsedTime);
	void RelativePosCamera();//�v���C���[���J����
	void TargetCamera(float elapsed_time);//�G�l�~�[���b�N�I��
	void FreeCamera();
	void SetVibration(float range, float timer); //�U���J�n
	void Vibrate(float elapsedTime);    //�U���J���� 
	void ClearCamera(float elapsedTime);
	void OverCamera(float elapsedTime);
	//�Z�b�^�[
	void SetTarget(const DirectX::XMFLOAT3& target_pos);
	void SetTargetAngle(const DirectX::XMFLOAT3& target_angle);
	void SetEnemy(const DirectX::XMFLOAT3& enemy_pos);
	void SetEye(const DirectX::XMFLOAT3 eye) { this->eye = eye; }
	void SetFocus(const DirectX::XMFLOAT3 focus) { this->focus = focus; }
	void SetUp(const DirectX::XMFLOAT3 up) { this->up = up; }
	void SetTargetFlag(const bool target) { targetFlag = target; }
	void SetLerpFlag(const bool lerp) { lerpFlag = lerp; }
	void SetFreeFlag(const bool free) { freeFlag = free; }
	void SetCameraCubeMax(const DirectX::XMFLOAT3 max) { cubeMax = max; }
	void SetCameraCubeMin(const DirectX::XMFLOAT3 min) { cubeMin = min; }
	void SetCameraCubePos(const DirectX::XMFLOAT3 pos) { cubePos = pos; }
	//�Q�b�^�[
	DirectX::XMFLOAT3& GetEye() { return eye; }
	DirectX::XMFLOAT3& GetFocus() { return focus; }
	DirectX::XMFLOAT3& GetUp() { return up; }
	const DirectX::XMFLOAT3& GetEnemyPos() const { return enemyPos; }
	const DirectX::XMFLOAT3& GetTargetPos() const { return targetPos; }
	const DirectX::XMFLOAT3& GetFront() const { return front; }
	const DirectX::XMFLOAT3& GetRight() const { return right; }
	const DirectX::XMFLOAT4X4& GetView()const { return view; }
	const DirectX::XMFLOAT4X4& GetProjection()const { return projection; }
	const bool& GetTargetFlag()const { return targetFlag; }
	const bool& GetLerpFlag()const { return lerpFlag; }
	const bool& GetFreeFlag()const { return freeFlag; }
	const AABB& GetCube()const { return cube; }
	static Camera& GetInstance()
	{
		return *camera;
	}
	void Destroy()
	{
		delete camera;
		camera = nullptr;
	}
	static void SetViewPort(ID3D11DeviceContext* context)
	{
		//��ʃT�C�Y�擾�̂��߃r���[�|�[�g���擾
		D3D11_VIEWPORT viewport;
		UINT num_viewports = 1;
		context->RSGetViewports(&num_viewports, &viewport);
		viewPort = DirectX::XMFLOAT2(viewport.Width, viewport.Height);
	}
};
#define pCamera (Camera::GetInstance())