#include "SceneGame.h"
#include "Fadeout.h"
#include "FbxLoad.h"
#include "Camera.h"
#include "LoadModel.h"
#include "Particle.h"
#include "HitAreaRnder.h"
#include "gamepad.h"
#include "KeyInput.h"
#include "easing.h"
#include "EffectManager.h"
#define SHADOW_SIZE_W 10000
#define SHADOW_SIZE_H 10000
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

void SceneGame::LightInit(ID3D11Device* device)
{
	Camera& camera = Camera::GetInstance();

	Light::Init(device);
	lightBuffer = std::make_unique<ConstantBuffer<CbLight>>(device);
	lightBuffer->data.ambientColor = Light::Ambient;
	lightBuffer->data.lightDir = Light::LightDir;
	lightBuffer->data.lightColor = Light::DirLightColor;
	lightBuffer->data.eyePos.x = camera.GetEye().x;
	lightBuffer->data.eyePos.y = camera.GetEye().y;
	lightBuffer->data.eyePos.z = camera.GetEye().z;
	lightBuffer->data.eyePos.w = 1.0f;

}
void SceneGame::LightUpdate(float elapsed_time)
{
	Camera& camera = Camera::GetInstance();

	float XM_PI = 3.141592654f;
	static float lightAngle = XM_PI;
	//Light::SetAmbient(VECTOR3(0.1f, 0.1f, 0.1f));

	LightDir.x = sinf(lightAngle);
	LightDir.y = 0.0f;
	LightDir.z = cosf(lightAngle);


	static float angle = XM_PI / 4;
	angle += elapsed_time;
	float s = sinf(angle) * 1.0f;
	float c = cosf(angle) * 1.0f;
	DirectX::XMFLOAT3 p = player->GetObj()->GetPosition();
	DirectX::XMFLOAT3 e[THUNDERNUM];

	for (int i = 0; i < THUNDERNUM; i++)
	{
		e[i] = bossEnemy->GetThunderData().pos[i];
		Light::SetPointLight(i, VECTOR3(e[i].x, 5, e[i].z), VECTOR4(0.3f, 0.6f, 1.0f, bossEnemy->GetThunderData().w[i]), range);
	}
	//Light::SetPointLight(1, VECTOR3(e.x, 30, e.z), VECTOR4(lightColor), range);
	//Light::SetSpotLight(0, VECTOR3(p.x, 30, p.z), VECTOR4(1, 0, 0, 10), lightDir, 40, neartst, 30 * 0.01745f);
	//Light::SetSpotLight(1, VECTOR3(e.x, 30, e.z), VECTOR4(1, 1, 0, 10), lightDir, 40, neartst, 30 * 0.01745f);
	//Light::SetSpotLight(2, VECTOR3(80, 30, 80), VECTOR4(lightColor), lightDir, 40, neartst, 30 * 0.01745f);
	//Light::SetSpotLight(3, VECTOR3(p.x, 30, p.z), VECTOR4(1.0f,0.0f,0.0f,1.f),LightDir, 80, neartst, fartst);
	//Light::SetSpotLight(2,pos, VECTOR4(0, 0, 1,1),dir,range, neartst, fartst);
	/*Light::SetSpotLight(2,pos, VECTOR3(1, 0, 0),dir,range, neartst, fartst);
	Light::SetSpotLight(3,pos, VECTOR3(0, 1, 0),dir,range, neartst, fartst);
	Light::SetSpotLight(4,pos, VECTOR3(1, 1, 0),dir,range, neartst, fartst);*/
	//lightBuffer->data.lightColor = Light::DirLightColor;
	lightBuffer->data.eyePos.x = camera.GetEye().x;
	lightBuffer->data.eyePos.y = camera.GetEye().y;
	lightBuffer->data.eyePos.z = camera.GetEye().z;
	lightBuffer->data.eyePos.w = 1.0f;

	memcpy(lightBuffer->data.PointLight, Light::PointLight, sizeof(POINTLIGHT) * Light::POINTMAX);
	memcpy(lightBuffer->data.SpotLight, Light::SpotLight, sizeof(SPOTLIGHT) * Light::SPOTMAX);
}
SceneGame::SceneGame(ID3D11Device* device, HWND hwnd)
{
	tutorialTex = std::make_unique<Sprite>(device, L"Data/images/tyu2.png");
	keyTex = std::make_unique<Sprite>(device, L"Data/images/button01.png");
	nowLoading = std::make_unique<Sprite>(device, L"Data/images/NOW_LOADING.png");
	fade = std::make_unique<Sprite>(device, L"Data/images/siro.png");
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);
			//モデルロード
			pLoadModel.Create();
			//	pLoadModel.Load(device, "Data/fbx/player04.fbx", "Player");
			pLoadModel.Load(device, "Data/fbx/tstStage/3/mobile_skull_09_1.fbx", "Skull");
			pLoadModel.Load(device, "Data/fbx/player1/player1.mdl", "Player");
			pLoadModel.Load(device, "Data/fbx/player1/weapon2.fbx", "PlayerWeapon");
			//	pLoadModel.Load(device, "Data/mdl/tst/mdl/tst.mdl", "TSTPLAYER");
				//pLoadModel.Load(device, "Data/fbx/ttt.mdl", "TSTPLAYER");
			pLoadModel.Load(device, "Data/fbx/enemy/enemy04.fbx", "Enemy");
			pLoadModel.Load(device, "Data/fbx/Rattack.fbx", "Rattack");

		/*	tstbox = std::make_unique<Character>(pLoadModel.GetModelResource("tst"));
			tstbox->SetPosition(DirectX::XMFLOAT3(0.0f, 5.0f, 0.0f));
			tstbox->SetScale(DirectX::XMFLOAT3(5.0f, 5.0f, 5.0f));
			tstbox->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
			tstbox1 = std::make_unique<Character>(pLoadModel.GetModelResource("tst"));
			tstbox1->SetPosition(DirectX::XMFLOAT3(0.0f, 5.0f, 0.0f));
			tstbox1->SetScale(DirectX::XMFLOAT3(5.0f, 5.0f, 5.0f));
			tstbox1->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));*/

			tstStage = std::make_shared<Stage>(device);

			player = std::make_unique<Player>(pLoadModel.GetModelResource("Player"), pLoadModel.GetModelResource("PlayerWeapon"));
			bossEnemy = std::make_unique<Enemy>(pLoadModel.GetModelResource("Enemy"));

			//軌跡
			trajectory = std::make_unique<Trajectory>(device, L"Data/images/SwordLine01.png");
			//シャドウ
			shadowmap = std::make_unique<FrameBuffer>(device, 1024 * 5, 1024 * 5, false/*enable_msaa*/, 1, DXGI_FORMAT_UNKNOWN/*not needed*/, DXGI_FORMAT_R32_TYPELESS);
			frameBuffer[0] = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
			frameBuffer[1] = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
			renderEffects = std::make_unique<RenderEffects>(device);

			//HP
			playerHp = std::make_unique<Ui>(device, L"Data/images/PlayerHp01.png");
			playerHpMax = std::make_unique<Sprite>(device, L"Data/images/PlayerHpFrame01.png");
			enemyHp = std::make_unique<Ui>(device, L"Data/images/PlayerHp01.png");
			enemyHpMax = std::make_unique<Sprite>(device, L"Data/images/PlayerHpFrame01.png");
			//SP
			playerSp = std::make_unique<Ui>(device, L"Data/images/PlayerSp01.png");
			playerSpMax = std::make_unique<Sprite>(device, L"Data/images/PlayerSpFrame01.png");

			//背景
			skyTexture = std::make_shared<Texture>();
			skyTexture->Load(device, L"Data/images/aaas.jpg");
			skyCube = std::make_unique<TexMeshObj>();
			skyCube->Create(device, VECTOR3(2000, 1000, 2000), VECTOR3(0, 0, 0), VECTOR3(0, 300, 0), TexMeshObj::ObjectType::GeometricCube, skyTexture);
			rasterizer = std::make_unique<RasterizerState>();
			rasterizer->CreateRasterizerState(device);
			samplerWrap = std::make_shared<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);

			bloomEffect = std::make_unique<Bloom>(device, 1920, 1080);
			vignetteEffect = std::make_unique<Vignette>(device, 1920, 1080);
			//カメラ
			length = DirectX::XMFLOAT3(55, 2, 55);
			Camera::GetInstance().Create(DirectX::XMFLOAT3(0.f, 50.0f, 130.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
			Camera::GetInstance().CalculateTransforms();
			player->GetObj()->SetHeadPosition();
			bossEnemy->GetObj()->SetHeadPosition();
			Camera::GetInstance().SetTarget(player->GetObj()->GetHeadPosition());
			Camera::GetInstance().SetTargetFlag(true);

			LightInit(device);
			modelRenderer = std::make_unique<ModelRenderer>(device, false);
			EfectModelRenderer = std::make_unique<ModelRenderer>(device, true);
			pParticleManager->Create(device);
			pHitAreaRender.Create(device);

			AttackLine::GetInctance().Create(device);
			vibrationRange = 0.01f;
			vibrationTime = 0.1f;
			state = STATE::GAME;
			firstFlag = true;
			clearFlag = false;
			overFlag = false;

			shader = std::make_unique<ShaderEx>();
			shader->CreateSprite(device);
		}, device);
	//BlendState
	blendGame[0] = std::make_unique<BlendState>(device, BLEND_MODE::ALPHA);
	blendGame[1] = std::make_unique<BlendState>(device, BLEND_MODE::ADD);
	//サウンドの初期化
	SoundManager::getinctance().Create(hwnd);
	SoundManager::getinctance().CreateSoundSourceGame();
#ifdef USE_IMGUI
	hitRenderFlag = true;
#else
	hitRenderFlag = false;

#endif // USE_IMGUI

	//エフェクトテスト
	{
		EffectObj::GetInstance().Create();
		EffectObj::GetInstance().Init(device);
		//e.Init(device, deviceContext);
	}
}
void SceneGame::Imgui()
{
#ifdef USE_IMGUI
	ImGui::Begin("r");
	ImGui::Image(shadowmap->GetDepthStencilShaderResourceView().Get(), ImVec2(200, 200));
	ImGui::End();
	ImGui::Begin("GAME");
	DirectX::XMFLOAT3 trajectoryColor = trajectory->GetColor();
	ImGui::ColorEdit3(u8"軌跡色", (float*)&trajectoryColor);
	trajectory->SetColor(trajectoryColor);
	ImGui::ShowDemoWindow();

	if (ImGui::CollapsingHeader("SCENE"))
	{
		if (ImGui::Button("title_open"))
		{
			pFadeOut.MoveStart();
		}
		if (ImGui::Button("clear_open"))
		{
			bossEnemy->GetObj()->SetExist(false);
			pFadeOut.MoveStart();
		}
		if (ImGui::Button("over_open"))
		{
			player->GetObj()->SetExist(false);
			pFadeOut.MoveStart();
		}
	}
	if (ImGui::Button(u8"当たり判定表示"))
	{
		hitRenderFlag = !hitRenderFlag;
	}
	//Stage
	if (ImGui::CollapsingHeader("stage"))
	{
		tstStage->Imgui();
	}
	//StageManager::getInstance().Imgui();
	//sound
	if (ImGui::CollapsingHeader("SOUND"))
	{
		SoundManager::getinctance().Imgui();
	}
	//エフェクト
	if (ImGui::CollapsingHeader("EFFECT"))
	{
		ImGui::Text("pos[0]x%.f", EffectObj::GetInstance().GetPosition(EffectObj::TYPE::THUNDER).x);
		ImGui::Text("pos[0]y%.f", EffectObj::GetInstance().GetPosition(EffectObj::TYPE::THUNDER).y);
		ImGui::Text("pos[0]z%.f", EffectObj::GetInstance().GetPosition(EffectObj::TYPE::THUNDER).z);

		ImGui::Text("pos[1]x%.f", EffectObj::GetInstance().GetPosition(EffectObj::TYPE::BLACKMIST).x);
		ImGui::Text("pos[1]y%.f", EffectObj::GetInstance().GetPosition(EffectObj::TYPE::BLACKMIST).y);
		ImGui::Text("pos[1]z%.f", EffectObj::GetInstance().GetPosition(EffectObj::TYPE::BLACKMIST).z);
		if (ImGui::Button(u8"birbiri"))
		{
			// エフェクトの再生
			EffectObj::GetInstance().SetPosition(EffectObj::TYPE::BRIBIRI, DirectX::XMFLOAT3(tstPos));
			EffectObj::GetInstance().SetScale(EffectObj::TYPE::BRIBIRI, DirectX::XMFLOAT3(tstScale));
			EffectObj::GetInstance().SetColor(EffectObj::TYPE::BRIBIRI, DirectX::XMFLOAT4(tstColor));
			EffectObj::GetInstance().Play(EffectObj::TYPE::BRIBIRI);
		}
		if (ImGui::Button(u8"雷"))
		{
			// エフェクトの再生
			EffectObj::GetInstance().SetPosition(EffectObj::TYPE::THUNDER, DirectX::XMFLOAT3(0, 0, 0));
			EffectObj::GetInstance().SetScale(EffectObj::TYPE::THUNDER, DirectX::XMFLOAT3(5, 5, 5));
			EffectObj::GetInstance().Play(EffectObj::TYPE::THUNDER);
			//	e.Play(EffectObj::TYPE::THUNDER);
		}
		if (ImGui::Button(u8"もやもや"))
		{
			// エフェクトの再生
			EffectObj::GetInstance().SetScale(EffectObj::TYPE::BLACKMIST, DirectX::XMFLOAT3(10, 40, 10));
			EffectObj::GetInstance().Play(EffectObj::TYPE::BLACKMIST);
			//e.Play(EffectObj::TYPE::BLACK);
		}
		if (ImGui::Button("CHARGE"))
		{
			EffectObj::GetInstance().Play(EffectObj::TYPE::CHARGE);
		}
		if (ImGui::Button("SLASHING"))
		{
			EffectObj::GetInstance().SetScale(EffectObj::TYPE::SLASHING, tstScale);
			EffectObj::GetInstance().SetColor(EffectObj::TYPE::SLASHING, tstColor);
			EffectObj::GetInstance().SetAngle(EffectObj::TYPE::SLASHING, tstAngle);
			EffectObj::GetInstance().SetPosition(EffectObj::TYPE::SLASHING, tstPos);
			EffectObj::GetInstance().SetStraightFlag(true);
		}
		if (ImGui::Button("CIRCLE"))
		{
			// エフェクトの再生

			EffectObj::GetInstance().SetScale(EffectObj::TYPE::CIRCLE, DirectX::XMFLOAT3(30, 30, 30));
			EffectObj::GetInstance().SetColor(EffectObj::TYPE::CIRCLE, DirectX::XMFLOAT4(1, 1, 1, 1));
			EffectObj::GetInstance().SetPosition(EffectObj::TYPE::CIRCLE, DirectX::XMFLOAT3(0, 0.1f, 0));
			EffectObj::GetInstance().Play(EffectObj::TYPE::CIRCLE);
			//e.Play(EffectObj::TYPE::BLACK););
		}
		if (ImGui::Button("TST"))
		{
			// エフェクトの再生

			EffectObj::GetInstance().SetScale(EffectObj::TYPE::TST, tstScale);
			EffectObj::GetInstance().SetColor(EffectObj::TYPE::TST, tstColor);
			EffectObj::GetInstance().SetPosition(EffectObj::TYPE::TST, tstPos);
			EffectObj::GetInstance().Play(EffectObj::TYPE::TST);
			//e.Play(EffectObj::TYPE::BLACK););
		}
		if (ImGui::Button("stopeffect"))
		{
			// エフェクトの停止
			EffectObj::GetInstance().Stop(EffectObj::TYPE::BLACKMIST);
		}
	}
	//カメラ
	if (ImGui::CollapsingHeader("CAMERA"))
	{
		if (ImGui::CollapsingHeader(u8"camera当たり判定"))
		{
			static DirectX::XMFLOAT3 cubeMax;
			static DirectX::XMFLOAT3 cubeMin;
			static DirectX::XMFLOAT3 cubePos;
			if (ImGui::CollapsingHeader(u8"Max"))
			{
				ImGui::InputFloat("cubeMax_x", &cubeMax.x, 0.1f);
				ImGui::InputFloat("cubeMax_y", &cubeMax.y, 0.1f);
				ImGui::InputFloat("cubeMax_z", &cubeMax.z, 0.1f);
				Camera::GetInstance().SetCameraCubeMax(cubeMax);
			}
			if (ImGui::CollapsingHeader(u8"Min"))
			{
				ImGui::InputFloat("cubeMin_x", &cubeMin.x, 0.1f);
				ImGui::InputFloat("cubeMin_y", &cubeMin.y, 0.1f);
				ImGui::InputFloat("cubeMin_z", &cubeMin.z, 0.1f);
				Camera::GetInstance().SetCameraCubeMin(cubeMin);
			}
			if (ImGui::CollapsingHeader("Pos"))
			{
				ImGui::InputFloat("cubePos_x", &cubePos.x, 0.1f);
				ImGui::InputFloat("cubePos_y", &cubePos.y, 0.1f);
				ImGui::InputFloat("cubePos_z", &cubePos.z, 0.1f);
				Camera::GetInstance().SetCameraCubePos(cubePos);
			}
		}
		static float foo = 1.0f;
		if (ImGui::Button(u8"フリーカメラ切り替え"))
		{
			static bool freeFlag = Camera::GetInstance().GetFreeFlag();
			freeFlag = !freeFlag;
			Camera::GetInstance().SetFreeFlag(freeFlag);
		}
		if (ImGui::CollapsingHeader(u8"Vibration"))
		{
			ImGui::InputFloat("VibrationTime", &vibrationTime, 0.1f);
			ImGui::InputFloat("VibrationRange", &vibrationRange, 0.01f);
		}
		ImGui::InputFloat("lengthX", &length.x, 5.f, 0, "%.f");
		ImGui::InputFloat("lengthY", &length.y, 5.f, 0, "%.f");
		ImGui::InputFloat("lengthZ", &length.z, 5.f, 0, "%.f");
		ImGui::Text("FoV%.f", Camera::GetInstance().FoV);

		ImGui::Text("eyeX%.f", Camera::GetInstance().GetEye().x);
		ImGui::Text("eyey%.f", Camera::GetInstance().GetEye().y);
		ImGui::Text("eyez%.f", Camera::GetInstance().GetEye().z);

		ImGui::Text("angleX%.f", Camera::GetInstance().angle.x);
		ImGui::Text("angleY%.f", Camera::GetInstance().angle.y);
		ImGui::Text("angleZ%.f", Camera::GetInstance().angle.z);

		ImGui::Text("FoV%.f", Camera::GetInstance().FoV);

		ImGui::InputFloat("waite%.f", &Camera::GetInstance().waiteFoV, 1.f);
		ImGui::InputFloat("moveFoV0%.f", &Camera::GetInstance().attackFoV[0], 1.f);
		ImGui::InputFloat("moveFoV1%.f", &Camera::GetInstance().attackFoV[1], 1.f);
		ImGui::InputFloat("moveFoV2%.f", &Camera::GetInstance().attackFoV[2], 1.f);
		ImGui::InputFloat("runFoV%.f", &Camera::GetInstance().runFoV, 1.f);
	}
	//LIGHT
	if (ImGui::CollapsingHeader("LIGHT"))
	{
		ImGui::InputFloat("lightx", &lightBuffer->data.lightDir.x, -1, 1);
		ImGui::InputFloat("lighty", &lightBuffer->data.lightDir.y, -1, 1);
		ImGui::InputFloat("lightz", &lightBuffer->data.lightDir.z, -1, 1);
		ImGui::InputFloat("lightw", &lightBuffer->data.lightDir.w, -1, 1);

		ImGui::ColorEdit3(u8"環境光", (float*)&lightBuffer->data.ambientColor);
		ImGui::ColorEdit3(u8"平行光", (float*)&lightBuffer->data.lightColor);

		ImGui::SliderFloat("pos.x", &lightPos.x, -50, 50);
		ImGui::SliderFloat("pos.y", &lightPos.y, -100, 100);
		ImGui::SliderFloat("pos.z", &lightPos.z, -50, 50);

		ImGui::SliderFloat("dir.x", &lightDir.x, -1, 1);
		ImGui::SliderFloat("dir.y", &lightDir.y, -1, 1);
		ImGui::SliderFloat("dir.z", &lightDir.z, -1, 1);

		ImGui::SliderFloat("range", &range, 0, 100);
		ImGui::SliderFloat("near", &neartst, 0, 100);
		ImGui::SliderFloat("fartst", &fartst, 0, 45.0f);
		//fartst *= 0.01745f;
		ImGui::SliderFloat("colorR", &lightColor.x, 0, 10);
		ImGui::SliderFloat("colorG", &lightColor.y, 0, 10);
		ImGui::SliderFloat("colorB", &lightColor.z, 0, 10);
		ImGui::SliderFloat("colorA", &lightColor.w, 0, 100.0f);

		float bloomThreshold = bloomEffect->GetGrowExtractionThreshold();
		ImGui::InputFloat(u8"しきい値", &bloomThreshold, 0.1f);
		bloomEffect->SetGrowExtractionThreshold(bloomThreshold);
		if (ImGui::CollapsingHeader("vignetteEffect"))
		{
			float area;
			ImGui::InputFloat("area", &area, 0.1f);
			vignetteEffect->SetVignetteArea(area);
			float smooth;
			ImGui::InputFloat("smooth", &smooth, 0.1f);
			vignetteEffect->SetVignetteSmooth(smooth);

			DirectX::XMFLOAT4 vignetteColor;
			ImGui::InputFloat("vignetteColor_x", &vignetteColor.x, 0.1f);
			ImGui::InputFloat("vignetteColor_y", &vignetteColor.y, 0.1f);
			ImGui::InputFloat("vignetteColor_z", &vignetteColor.z, 0.1f);
			ImGui::InputFloat("vignetteColor_w", &vignetteColor.w, 0.1f);
			vignetteEffect->SetVignetteColor(vignetteColor);
		}
	}
	//Fog
	static float fogNear;
	static float fogFar;
	static DirectX::XMFLOAT4 fogColor;
	if (ImGui::CollapsingHeader("FOG"))
	{
		ImGui::InputFloat("fogNear", &fogNear, 1.f);
		modelRenderer->SetFogNear(fogNear);
		skyCube->SetFogNear(fogNear);

		ImGui::InputFloat("fogFar", &fogFar, 1.f);
		modelRenderer->SetFogFar(fogFar);
		skyCube->SetFogFar(fogFar);

		ImGui::ColorEdit3("dissolveColor", (float*)&fogColor);
		modelRenderer->SetFogColor(fogColor);
		skyCube->SetFogColor(fogColor);
	}
	//調整
	if (ImGui::CollapsingHeader(u8"調整用"))
	{
		if (ImGui::CollapsingHeader(u8"0"))
		{
			ImGui::InputFloat("tstPosx", &tstPos.x, 0.1f);
			ImGui::InputFloat("tstPosy", &tstPos.y, 0.1f);
			ImGui::InputFloat("tstPosz", &tstPos.z, 0.1f);
			//ImGui::ColorEdit3("tstColor", (float*)&tstColor);
			ImGui::InputFloat("tstColor.x", &tstColor.x, 0.1f);
			ImGui::InputFloat("tstColor.y", &tstColor.y, 0.1f);
			ImGui::InputFloat("tstColor.z", &tstColor.z, 0.1f);
			ImGui::InputFloat("tstColor.w", &tstColor.w, 0.1f);
			ImGui::InputFloat("tstScalex", &tstScale.x, 0.1f);
			ImGui::InputFloat("tstScaley", &tstScale.y, 0.1f);
			ImGui::InputFloat("tstScalez", &tstScale.z, 0.1f);

			ImGui::InputFloat("scale", &easingScale, 0.1f);
			ImGui::InputFloat("color", &easingColor, 0.1f);
			ImGui::InputFloat("tstAnglex", &tstAngle.x, 0.1f);
			ImGui::InputFloat("tstAngley", &tstAngle.y, 0.1f);
			ImGui::InputFloat("tstAnglez", &tstAngle.z, 0.1f);
		}
		if (ImGui::CollapsingHeader(u8"1"))
		{
			ImGui::InputFloat("tstPos1x", &tstPos1.x, 0.1f);
			ImGui::InputFloat("tstPos1y", &tstPos1.y, 0.1f);
			ImGui::InputFloat("tstPos1z", &tstPos1.z, 0.1f);
			//ImGui::ColorEdit3("tstColor1", (float*)&tstColor1);
			ImGui::InputFloat("tstColor1.x", &tstColor1.x, 0.1f);
			ImGui::InputFloat("tstColor1.y", &tstColor1.y, 0.1f);
			ImGui::InputFloat("tstColor1.z", &tstColor1.z, 0.1f);
			ImGui::InputFloat("tstColor1.w", &tstColor1.w, 0.1f);
			ImGui::InputFloat("tstScale1x", &tstScale1.x, 0.1f);
			ImGui::InputFloat("tstScale1y", &tstScale1.y, 0.1f);
			ImGui::InputFloat("tstScale1z", &tstScale1.z, 0.1f);
			ImGui::InputFloat("scale1", &easingScale1, 0.1f);
			ImGui::SliderFloat("tstAngle1x", &tstAngle1.x, -3.14f, 3.14f);
			ImGui::SliderFloat("tstAngle1y", &tstAngle1.y, -3.14f, 3.14f);
			ImGui::SliderFloat("tstAngle1z", &tstAngle1.z, -3.14f, 3.14f);
		}
	}
	ImGui::End();
#endif
}
int SceneGame::Update(float elapsedTime)
{
	static bool fadeFlag = false;
	if (!tutorialFlag)
	{
		tutorialTime += elapsedTime;
		if (tutorialTime > 2.0f)
		{

			if (KeyInput::KeyTrigger() & KEY_ENTER || input::ButtonRisingState(0, input::PadLabel::A) && !IsNowLoading())
			{
				fadeFlag = true;
			}
			if (fadeFlag)
			{
				fadeColorW += elapsedTime;
				if (fadeColorW >= 1.0f)
				{
					tutorialFlag = true;
					tutorialTime = 0;
					fadeFlag = false;
				}
			}

		}
		return 0;
	}
	EndLoading();
	Imgui();
	skyCube->Update();
	LightUpdate(elapsedTime);
	DirectX::XMFLOAT3 angle = player->GetObj()->GetAngle();
	DirectX::XMFLOAT3 enmposition = player->GetObj()->GetPosition();
	static float volume = 1.0f;
	static float pan = 0.0f;
#ifdef USE_IMGUI
	if (KeyInput::KeyTrigger() & KEY_ENTER)
	{
		player->GetObj()->SetExist(false);
		pFadeOut.MoveStart();
	}
#endif

	if (pFadeOut.Update(elapsedTime))
	{
		SoundManager::getinctance().Stop(static_cast<int>(SoundManager::SOUNDGAME::GAME_BGM));
		if (state == STATE::CLEAR) return SceneName::CLEAR;
		else if (state == STATE::OVER) return SceneName::OVER;
		else return SceneName::TITLE;
	}
	switch (state)
	{
	case STATE::GAME:
		if (bossEnemy->attackStartFlag)
		{
			//BGMSTART
			SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::GAME_BGM), true);
			bossEnemy->attackStartFlag = false;
		}
		pHitAreaRender.Clear();
		//soundBGM->SetPan(pan);
		//soundBGM->SetVolume(volume);
		//tstcube->Update();
		tstStage->Update(elapsedTime);
		trajectory->SetStartPosition(player->GetTrajectoryStartPosition());
		trajectory->SetEndPosition(player->GetTrajectoryEndPosition());
		trajectory->SwapPosition();
		bossEnemy->Update(elapsedTime);
		//stage->CalculateTransform();
		AttackLine::GetInctance().Update();
		if (firstFlag)
		{
			EffectObj::GetInstance().SetScale(EffectObj::TYPE::THUNDER, DirectX::XMFLOAT3(3, 3, 3));
			EffectObj::GetInstance().SetColor(EffectObj::TYPE::THUNDER, DirectX::XMFLOAT4(0.5f, 1, 1, 1));
			EffectObj::GetInstance().Play(EffectObj::TYPE::THUNDER);
			firstFlag = false;
		}
		EffectObj::GetInstance().SetPosition(EffectObj::TYPE::BLACKMIST, bossEnemy->GetObj()->GetPosition());
		EffectObj::GetInstance().Update();

		player->Update(elapsedTime);
		playerHp->Update(elapsedTime, player->GetObj()->GetMaxHp(), player->GetObj()->GetHp(), 6.f);
		enemyHp->Update(elapsedTime, bossEnemy->GetObj()->GetMaxHp(), bossEnemy->GetObj()->GetHp(), 6.f);
		playerSp->Update(elapsedTime, player->GetObj()->GetMaxSp(), player->GetObj()->GetSp(), 6.f);

		CollisionManager::Judge(elapsedTime, *player->GetObj(), *bossEnemy->GetObj(), *tstStage);
		Camera::GetInstance().SetTarget(DirectX::XMFLOAT3(player->GetObj()->GetPosition().x, player->GetObj()->GetHeadPosition().y, player->GetObj()->GetPosition().z));
		pParticleManager->Update(elapsedTime);
		//カメラ変更
		if (!Camera::GetInstance().GetFreeFlag())
		{
			if (KeyInput::KeyTrigger() & KEY_L || input::ButtonRisingState(0, input::PadLabel::RTHUMB))
			{
				if (!Camera::GetInstance().GetTargetFlag())
				{
					Camera::GetInstance().SetTargetFlag(true);
				}
				else
				{
					Camera::GetInstance().SetTargetFlag(false);
					Camera::GetInstance().SetLerpFlag(true);
				}
			}
			if (Camera::GetInstance().GetTargetFlag())
			{
				//length = DirectX::XMFLOAT3(40, 10, 40);
				Camera::GetInstance().SetCamera(length, Camera::TYPE::TARGET);
			}
			else
			{
				//length = DirectX::XMFLOAT3(40, 5, 40);
				Camera::GetInstance().SetCamera(length, Camera::TYPE::RELATIVE_POS);
			}
		}
		else
		{
			Camera::GetInstance().SetCamera(length, Camera::TYPE::FREE);
		}
		//オーバー
		if (!player->GetObj()->GetExist())
		{
			state = STATE::OVER;
			bool lerpFlag = Camera::GetInstance().GetLerpFlag();
			lerpFlag = !lerpFlag;
			Camera::GetInstance().SetLerpFlag(lerpFlag);
		}
		//クリア
		else if (!bossEnemy->GetObj()->GetExist())
		{
			state = STATE::CLEAR;
		}
		//ストップ、カメラシェイク
		if (player->GetObj()->stopFlag)
		{
			state = STATE::STOP;
		}
		break;
	case STATE::STOP:
		static float  timer;
		timer++;
		if (timer >= player->GetAttackData()->stopTime[3])
		{
			timer = 0;
			player->GetObj()->stopFlag = false;
			Camera::GetInstance().SetVibration(vibrationRange, vibrationTime);
			state = STATE::GAME;
		}
		break;
	case STATE::CLEAR:
		bossEnemy->Update(elapsedTime);
		//nlength = { 85,40,85 };
		Camera::GetInstance().SetCamera(length, Camera::TYPE::CLEAR);
		//Camera::GetInstance().Updata(elapsed_time);
		ParticleManager::getInstance()->Update(elapsedTime);
		EffectObj::GetInstance().Update();
		break;
	case STATE::OVER:
		player->Update(elapsedTime);
		bossEnemy->Update(elapsedTime * 0.2f);
		//length = { 55,-8,55 };
		Camera::GetInstance().SetTargetAngle(player->GetObj()->GetAngle());
		Camera::GetInstance().SetCamera(length, Camera::TYPE::OVER);
		//Camera::GetInstance().Updata(elapsed_time);
		EffectObj::GetInstance().Update();
		break;
	}

	Camera::GetInstance().Updata(elapsedTime);
	Camera::GetInstance().CalculateTransforms();
	Camera::GetInstance().flag = true;
	pHitAreaRender.CalculateTransform(Camera::GetInstance().GetView(), Camera::GetInstance().GetProjection());
	return 0;
}
void SceneGame::Render(float elapsedTime, ID3D11DeviceContext* devicecontext)
{
	if (!tutorialFlag)
	{
		tutorialTex->Render(devicecontext, 0, 0, 1920, 1080, 0, 0, 1920, 1080, 0, 1, 1, 1, 1);
		if (IsNowLoading() && tutorialTime < 2.0f)nowLoading->Render(devicecontext, 50, 1000, 416, 32, 0, 0, 832, 64, 0, 1, 1, 1, 1);
		else
		{
			blendGame[0]->Activate(devicecontext);
			keyTex->Render(devicecontext, 50, 800, 84, 84, 0, 0, 84, 84, 0, 1, 1, 1, 1);
			blendGame[0]->Deactivate(devicecontext);
		}
		blendGame[0]->Activate(devicecontext);
		fade->Render(devicecontext, 0, 0, 1920, 1080, 0, 0, 1024, 1024, 0, 0, 0, 0, fadeColorW);
		blendGame[0]->Deactivate(devicecontext);
		return;
	}
	EndLoading();

	//ビュー行列
	const float PI = 3.14f;
	DirectX::XMFLOAT4X4 view, projection;
	view = Camera::GetInstance().GetView();
	projection = Camera::GetInstance().GetProjection();
	DirectX::XMMATRIX V;
	{
		V = DirectX::XMLoadFloat4x4(&view);
	}
	DirectX::XMMATRIX P;
	{
		P = DirectX::XMLoadFloat4x4(&projection);
	}
	FLOAT4X4 view_projection;
	{
		DirectX::XMMATRIX WVP;
		DirectX::XMMATRIX C = DirectX::XMMatrixSet(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
		WVP = C * V * P;
		DirectX::XMStoreFloat4x4(&view_projection, WVP);
	}
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	devicecontext->RSGetViewports(&num_viewports, &viewport);
	static float aspect = viewport.Width / viewport.Height;
	Camera lightCamera;
	lightCamera.SetPerspective(1000, aspect, 1.0f, 700);
	lightCamera.flag = false;
	VECTOR3 lFocus = VECTOR3(player->GetObj()->GetPosition().x, player->GetObj()->GetPosition().y, player->GetObj()->GetPosition().z);
	DirectX::XMVECTOR direction = DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&lightBuffer->data.lightDir));
	DirectX::XMVECTOR focus = DirectX::XMLoadFloat3(&VECTOR3(lFocus.x, lFocus.y, lFocus.z));
	VECTOR3 eye;
	DirectX::XMStoreFloat3(&eye, DirectX::XMVectorAdd(DirectX::XMVectorScale(direction, 250), focus));
	lightCamera.SetEye(VECTOR3(eye.x, eye.y, eye.z));
	lightCamera.SetFocus(lFocus);
	lightCamera.CalculateTransforms();
	DirectX::XMFLOAT4X4 lightView = lightCamera.GetView();
	DirectX::XMFLOAT4X4 lightProjection = lightCamera.GetProjection();
	FLOAT4X4 lightViewProjection;
	DirectX::XMStoreFloat4x4(&lightViewProjection, DirectX::XMLoadFloat4x4(&lightView) * DirectX::XMLoadFloat4x4(&lightProjection));
	//shadow
	rasterizer->SetRasterizerState(RS_CULL_FRONT, devicecontext);

	shadowmap->Clear(devicecontext);
	shadowmap->Activate(devicecontext);
	modelRenderer->ShadowBegin(devicecontext, lightViewProjection);
	modelRenderer->ShadowDraw(devicecontext, *player->GetModel());
	modelRenderer->ShadowDraw(devicecontext, *bossEnemy->GetModel());
	shadowmap->Deactivate(devicecontext);

	//サンプラー設定
	samplerWrap->Activate(devicecontext, 0);
	//カラーマップテクスチャ
	frameBuffer[0]->Clear(devicecontext);
	frameBuffer[0]->Activate(devicecontext);
	{
		lightBuffer->Activate(devicecontext, 3);//hlsl b(03)
		skyCube->Render(devicecontext, view, projection);
		rasterizer->SetRasterizerState(RS_CULL_BACK, devicecontext);
		blendGame[0]->Activate(devicecontext);
		modelRenderer->Begin(devicecontext, view_projection, VECTOR4(0, -1, -1, 1));
		modelRenderer->Draw(devicecontext, player->GetModel(), VECTOR4(0.5, 0.5, 0.5, 1));
		modelRenderer->Draw(devicecontext, player->GetWeaponModel());

		//stage
		if (tstStage->GetLeftObj1()->GetExist())modelRenderer->Draw(devicecontext, tstStage->GetLeftObj1()->GetModel());
		if (tstStage->GetLeftObj2()->GetExist())modelRenderer->Draw(devicecontext, tstStage->GetLeftObj2()->GetModel());
		if (tstStage->GetRightObj1()->GetExist())modelRenderer->Draw(devicecontext, tstStage->GetRightObj1()->GetModel());
		if (tstStage->GetRightObj2()->GetExist())modelRenderer->Draw(devicecontext, tstStage->GetRightObj2()->GetModel());
		if (tstStage->GetFrontObj1()->GetExist())modelRenderer->Draw(devicecontext, tstStage->GetFrontObj1()->GetModel());
		if (tstStage->GetFrontObj2()->GetExist())modelRenderer->Draw(devicecontext, tstStage->GetFrontObj2()->GetModel());
		if (tstStage->GetBackObj1()->GetExist())modelRenderer->Draw(devicecontext, tstStage->GetBackObj1()->GetModel());
		if (tstStage->GetBackObj2()->GetExist())modelRenderer->Draw(devicecontext, tstStage->GetBackObj2()->GetModel());
		if (tstStage->GetHaka1()->GetExist())modelRenderer->Draw(devicecontext, tstStage->GetHaka1()->GetModel());
		if (tstStage->GetHaka2()->GetExist())modelRenderer->Draw(devicecontext, tstStage->GetHaka2()->GetModel());
		modelRenderer->Draw(devicecontext, tstStage->GetFloor()->GetModel());
		modelRenderer->Draw(devicecontext, bossEnemy->GetModel(), VECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		if (bossEnemy->GetSkull()->GetExist())modelRenderer->Draw(devicecontext, bossEnemy->GetSkull()->GetModel(), VECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
		modelRenderer->End(devicecontext);

		if (hitRenderFlag)
		{
			pHitAreaRender.Render(devicecontext, DirectX::XMFLOAT4(0, -1, -1, 1));
		}

		blendGame[0]->Deactivate(devicecontext);
		if (AttackLine::GetInctance().GetExistFlag())
		{
			AttackLine::GetInctance().Render(devicecontext, view_projection, DirectX::XMFLOAT4(1, 0, 0, 0.5f));
		}
		//Effekseer描画テスト
		rasterizer->SetRasterizerState(RS_CULL_BACK, devicecontext);
		EffectObj::GetInstance().Render();
		//trajectory->Render(devicecontext, view_projection);
		blendGame[0]->Deactivate(devicecontext);

		blendGame[1]->Activate(devicecontext);
		//軌跡描画
		if (player->GetObj()->GetAttackFlag())
			trajectory->Render(devicecontext, view_projection);

		pParticleManager->Render(devicecontext, V, P, DirectX::XMFLOAT4(0, -1, -1, 1));
		//エフェクトテスト
#if 1
		if (bossEnemy->GetObj()->GetRAttackFlag())
		{
			EfectModelRenderer->Begin(devicecontext, view_projection, VECTOR4(0, -1, -1, 1));
			EfectModelRenderer->Draw(devicecontext, bossEnemy->GetRAttackObj1()->GetModel(),VECTOR4(0,0,1, bossEnemy->GetEffectColorW1()));
			EfectModelRenderer->Draw(devicecontext, bossEnemy->GetRAttackObj2()->GetModel(), VECTOR4(0, 0, 1, bossEnemy->GetEffectColorW2()));
			EfectModelRenderer->End(devicecontext);
		}
#else
		EfectModelRenderer->Begin(devicecontext, view_projection, VECTOR4(0, -1, -1, 1));
		EfectModelRenderer->Draw(devicecontext, tstbox->GetModel(), tstColor);
		EfectModelRenderer->Draw(devicecontext, tstbox1->GetModel(), tstColor1);
		EfectModelRenderer->End(devicecontext);
#endif
		blendGame[1]->Deactivate(devicecontext);
		samplerWrap->Deactivate(devicecontext);
		lightBuffer->DeActivate(devicecontext);
	}

	frameBuffer[0]->Deactivate(devicecontext);
	rasterizer->SetRasterizerState(RS_CULL_BACK, devicecontext);
	frameBuffer[1]->Clear(devicecontext);
	frameBuffer[1]->Activate(devicecontext);

	renderEffects->ShadowRender(devicecontext, frameBuffer[0]->GetRenderTargetShaderResourceView().Get(), frameBuffer[0]->GetDepthStencilShaderResourceView().Get(), shadowmap->GetDepthStencilShaderResourceView().Get(), view, projection, lightCamera.GetView(), lightCamera.GetProjection());

	bloomEffect->Generate(devicecontext, frameBuffer[0]->GetRenderTargetShaderResourceView().Get());
	bloomEffect->Blit(devicecontext);

	frameBuffer[1]->Deactivate(devicecontext);
	//depthに書きこみきったものをfrontに描画
	//追加するならframebufferを追加し最終書き込み終わったものを取得framebuffer[1]と同じ手順
	if (player->GetObj()->GetHitStateFlag())
	{
		float area = vignetteEffect->GetVignetteArea();
		area += 15.f * elapsedTime;
		vignetteEffect->SetVignetteArea(area);
		vignetteEffect->Render(devicecontext, frameBuffer[1]->GetRenderTargetShaderResourceView().Get());
	}
	else
	{
		vignetteEffect->SetVignetteArea(1.5f);
		playerHpMax->Render(devicecontext, frameBuffer[1]->GetRenderTargetShaderResourceView().Get(), 0, 0, 1920, 1080, 0, 0, 1920, 1080, 0, 1, 1, 1, 1);
	}
	/*vignetteEffect->Render(devicecontext, frameBuffer[0]->GetRenderTargetShaderResourceView().Get());*/


	blendGame[0]->Activate(devicecontext);
	//ＨＰ
	playerHpMax->Render(devicecontext, 180, 28, player->GetObj()->GetMaxHp() * 6.04f, 24, 0, 0, 640, 30, 0, 1, 1, 1, 1);
	playerHp->Render(devicecontext, DirectX::XMFLOAT2(183, 30), 20.f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(630, 18), 0, DirectX::XMFLOAT4(1, 1, 1, 1));
	enemyHpMax->Render(devicecontext, 670, 870, bossEnemy->GetObj()->GetMaxHp() * 6.04f, 24, 0, 0, 640, 30, 0, 1, 0, 0, 1);
	enemyHp->Render(devicecontext, DirectX::XMFLOAT2(673, 873), 20.f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(630, 18), 0, DirectX::XMFLOAT4(1, 0, 0, 1));
	//SP
	playerSpMax->Render(devicecontext, shader.get(), 180, 58, player->GetObj()->GetMaxHp() * 6.04f, 24, 0, 0, 500, 20, 0, 1, 1, 1, 1);
	if (player->GetObj()->GetTiredFlag())
		playerSp->Render(devicecontext, DirectX::XMFLOAT2(183, 60), 20.f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(494, 14), 0, DirectX::XMFLOAT4(1, 0, 0, 1));
	else
		playerSp->Render(devicecontext, DirectX::XMFLOAT2(183, 60), 20.f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(494, 14), 0, DirectX::XMFLOAT4(1, 1, 1, 1));
	pFadeOut.Render(devicecontext);

	blendGame[0]->Deactivate(devicecontext);
}
SceneGame::~SceneGame()
{
	SoundManager::getinctance().Destroy();
	Camera::GetInstance().Destroy();
	pLoadModel.Destory();
	pParticleManager->Destroy();
	pHitAreaRender.Destroy();
	EffectObj::GetInstance().Destory();
	AttackLine::GetInctance().Destroy();
	//e.~EffectObj();
}