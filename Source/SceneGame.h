#pragma once
#include "Scene.h"
#include<memory>
#include <thread>
#include <mutex>
#include "Character.h"
#include "ModelRenderer.h"
#include "Light.h"
#include "Character.h"
#include "Player.h"
#include "Enemy.h"
#include "Sampler.h"
#include "Trajectory.h"
#include "Texture.h"
#include "CollisionManager.h"
#include "Framebuffer.h"
#include "RenderEffects.h"
#include "RasterizerState.h"
#include "TexMeshObj.h"
#include "ObjMesh.h"
#include "Ui.h"
#include "Bloom.h"
#include "Vignette.h"
#include "Sound.h"
#include "AttackLine.h"
#include "BlendState.h"
#include "Stage.h"
class  SceneGame : public Scene
{
public:
	std::unique_ptr<std::thread> loading_thread;
	std::mutex loading_mutex;

	bool IsNowLoading()
	{
		if (loading_thread && loading_mutex.try_lock())
		{
			loading_mutex.unlock();
			return false;
		}
		return true;
	}
	void EndLoading()
	{
		if (loading_thread && loading_thread->joinable())
		{
			loading_thread->join();
		}
	}
	struct CbLight
	{
		VECTOR4 lightColor;
		VECTOR4 lightDir;
		VECTOR4 ambientColor;
		VECTOR4 eyePos;

		POINTLIGHT PointLight[Light::POINTMAX];
		SPOTLIGHT SpotLight[Light::SPOTMAX];
	};
	std::unique_ptr<ConstantBuffer<CbLight>> lightBuffer;
	VECTOR3 light;
	// ライト
	VECTOR3 LightDir;
	//light tst
	VECTOR3 lightPos = VECTOR3(0, 30, 0);
	VECTOR4 lightColor = VECTOR4(1, 1, 1, 10);
	VECTOR3 lightDir = VECTOR3(0, -1, 0);
	float range = 45;
	float neartst = 0;
	float fartst = 30.0;

	bool hitRenderFlag = true;

	void LightInit(ID3D11Device* device);
	void LightUpdate(float elapsed_time);

	SceneGame(ID3D11Device* device, HWND hwnd);
	void Imgui();
	int Update(float elapsed_time);
	//void RenderShadow(ID3D11DeviceContext* immediateContext);
	void Render(float elapsed_time, ID3D11DeviceContext* devicecontext);
	~SceneGame();
private:
	std::unique_ptr<BlendState> blendGame[2];
	std::unique_ptr<Sprite> nowLoading;
	enum class STATE
	{
		GAME,
		STOP,
		CLEAR,//クリア時演出後クリアシーンへ
		OVER, // オーバー時演出後オーバーシーンへ
	};

	bool clearFlag = false;
	bool overFlag = false;
	STATE state = STATE::GAME;

	float vibrationTime;
	float vibrationRange;
	//ポストエフェクト
	std::unique_ptr<Bloom> bloomEffect;
	std::unique_ptr<Vignette>vignetteEffect;
	//framebuffers
	std::unique_ptr<FrameBuffer> shadowmap;
	std::unique_ptr<FrameBuffer>frameBuffer[2];
	std::unique_ptr<RenderEffects>renderEffects;
	//std::unique_ptr<Texture> shadowMap = nullptr;
	//カメラ設定用
	DirectX::XMFLOAT3 length = DirectX::XMFLOAT3(0, 0, 0);
	//HP
	std::unique_ptr<Ui>playerHp;
	std::unique_ptr<Sprite>playerHpMax;
	std::unique_ptr<Ui>enemyHp;
	std::unique_ptr<Sprite>enemyHpMax;
	//SP
	std::unique_ptr<Ui>playerSp;
	std::unique_ptr<Sprite>playerSpMax;

	std::unique_ptr<ModelRenderer>modelRenderer;
	std::unique_ptr<ModelRenderer>EfectModelRenderer;//深度値をみない
	std::unique_ptr<Player>player;
	std::unique_ptr<Enemy>bossEnemy;
	std::unique_ptr<Trajectory>trajectory; //軌跡
	std::unique_ptr<Obj3D> stage;

	//tstStage
	std::shared_ptr<Stage> tstStage;
	//std::unique_ptr<AttackLine> attackLine;
	//サンプラー
	std::shared_ptr<Sampler>samplerWrap;
	//ラスタライザー
	std::unique_ptr<RasterizerState>rasterizer;

	std::shared_ptr<Texture> skyTexture = nullptr;
	std::unique_ptr<TexMeshObj>skyCube;

	std::shared_ptr<Texture> bisuko_texture = nullptr;
	std::unique_ptr<TexMeshObj>bisukoCube;

	//tst
	std::unique_ptr<OBJMesh>appear = nullptr;

	DirectX::XMFLOAT3 ppp = DirectX::XMFLOAT3(-10.f, 497, 1000);

	//調整用
	std::unique_ptr<Obj3D> tstbox;
	std::unique_ptr<Obj3D> tstbox1;
	//位置調整用
	DirectX::XMFLOAT3 tstPos = DirectX::XMFLOAT3(0.0f, 8.1f, -200.f);
	DirectX::XMFLOAT3 tstPos1 = DirectX::XMFLOAT3(0.0f, 2.5f, -200.f);
	//カラー調整用
	float easingColor = 4.f;
	VECTOR4 tstColor = VECTOR4(0, 0, 1, 1);
	VECTOR4 tstColor1 = VECTOR4(0, 0, 1, 1);
	//サイズ調整用
	float easingScale = 30;
	float easingScale1 = 30;
	DirectX::XMFLOAT3 tstScale = DirectX::XMFLOAT3(5, 25, 5);
	DirectX::XMFLOAT3 tstScale1 = DirectX::XMFLOAT3(16, 2.f, 16);
	//角度調整用

	DirectX::XMFLOAT3 tstAngle = DirectX::XMFLOAT3(0, 0.0f, 0.0f);
	DirectX::XMFLOAT3 tstAngle1 = DirectX::XMFLOAT3(0, 0.0f, 0.0f);

	bool effectFlag = false;
	bool firstFlag = false;
	bool resultflag = false;//true クリア　false オーバー

	//std::unique_ptr<SoundManager>					soundManager;
	//説明画像
	std::unique_ptr<Sprite>tutorialTex;
	std::unique_ptr<Sprite>keyTex;
	bool tutorialFlag = false;
	float tutorialTime = 0;
	std::unique_ptr<Sprite>fade;
	float fadeColorW = 0.f;

	//画像tst
	std::unique_ptr<Shader>shader;
};

