#pragma once
#include<DirectXMath.h>
#include <d3d11.h>

class Camera
{
private:
	
	bool lerpFlag = false;
	bool targetFlag = false;
	bool freeFlag = false;
	static Camera* camera;
	float time = 0.0f;  //振動時間
	float range = 0.0f; //振動幅
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
public:
	DirectX::XMFLOAT3 angle;
	//FoV変動
	bool   flag = false;
	float  FoVattenRate = 0.f; //FoVの減衰比率
	float  attackFoV[3];//プレイヤー攻撃時Fov
	float  runFoV;//プレイヤー移動時Fov
	float  waiteFoV = 0.0f;//プレイヤー停止時Fov
	float  FoV = 0.0f;

	float aspect;//アスペクト比
	float nearZ; // 表示最近面までの距離
	float farZ;//表紙最遠面までの距離
	enum TYPE
	{
		RELATIVE_POS,//通常プレイヤーカメラ左右移動あり
		TARGET,//エネミーロックオン
		FREE,//フリーカメラ
		CLEAR,//クリア時カメラ
		OVER,//オーバー時カメラ
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
	void SetCamera(const DirectX::XMFLOAT3& leng, const TYPE& type);//カメラ変更
	void CalculateTransforms();
	void Updata(float elapsedTime);
	void RelativePosCamera();//プレイヤー軸カメラ
	void TargetCamera(float elapsed_time);//エネミーロックオン
	void FreeCamera();
	void SetVibration(float range, float timer); //振動開始
	void Vibrate(float elapsedTime);    //振動カメラ 
	void ClearCamera(float elapsedTime);
	void OverCamera(float elapsedTime);
	//セッター
	void SetTarget(const DirectX::XMFLOAT3& target_pos);
	void SetTargetAngle(const DirectX::XMFLOAT3& target_angle);
	void SetEnemy(const DirectX::XMFLOAT3& enemy_pos);
	void SetEye(const DirectX::XMFLOAT3 eye) { this->eye = eye; }
	void SetFocus(const DirectX::XMFLOAT3 focus) { this->focus = focus; }
	void SetUp(const DirectX::XMFLOAT3 up) { this->up = up; }
	void SetTargetFlag(const bool target) { targetFlag = target; }
	void SetLerpFlag(const bool lerp) { lerpFlag = lerp; }
	void SetFreeFlag(const bool free) { freeFlag = free; }
	//ゲッター
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
		//画面サイズ取得のためビューポートを取得
		D3D11_VIEWPORT viewport;
		UINT num_viewports = 1;
		context->RSGetViewports(&num_viewports, &viewport);
		viewPort = DirectX::XMFLOAT2(viewport.Width, viewport.Height);
	}
};
#define pCamera (Camera::GetInstance())