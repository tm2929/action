#include "SceneTitle.h"
#include "Fadeout.h"
#include "BlendState.h"
#include "FbxLoad.h"
#include "LoadModel.h"
#include "Camera.h"
#include "KeyInput.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif
#include "gamepad.h"
#include "Texture.h"

SceneTitle::SceneTitle(ID3D11Device* device, HWND hwnd)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);
#ifdef USE_IMGUI
			//titleTex = std::make_unique<Sprite>(device, L"Data/images/bg.png");
			titleTex = std::make_unique<Sprite>(device, L"Data/images/Title01.png");
#else
			titleTex = std::make_unique<Sprite>(device, L"Data/images/Title01.png");
#endif
			samplerWrap = std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
			rasterizer = std::make_unique<RasterizerState>();
			rasterizer->CreateRasterizerState(device);
			colorChangeFlag = false;
			dissolveShader = std::make_unique<ShaderEx>();
			D3D11_TEXTURE2D_DESC tex2D;
			 load_texture_from_file(device, L"Data/dissolve/Dissolve_01_02.png", srv.GetAddressOf(), &tex2D);
			dissolveShader->Create(device, "Data/shaders/cso/sprite_vs.cso", "Data/shaders/cso/Dissolve_ps.cso");
		}, device);
	soundManager = std::make_unique<SoundManager>(hwnd);
	SoundManager::getinctance().Create(hwnd);
	soundManager->CreateSoundSourceTitle();
	soundManager->Play(static_cast<int>(SoundManager::SOUNDTITLE::TITLE_BGM), true);
	blend = std::make_unique<BlendState>(device, BLEND_MODE::ALPHA);
}

int SceneTitle::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return 0;
	}
	EndLoading();

	if (pFadeOut.Update(elapsed_time))
	{

		soundManager->Stop(static_cast<int>(SoundManager::SOUNDTITLE::TITLE_BGM));
		return SceneName::GAME;
	}
	if (KeyInput::KeyTrigger() & KEY_ENTER || input::ButtonRisingState(0, input::PadLabel::A))
	{
		pFadeOut.MoveStart();
		soundManager->Play(static_cast<int>(SoundManager::SOUNDTITLE::ENTER), false);
		colorChangeFlag = true;
	}
	static DirectX::XMFLOAT2 tst = { 0,0 };
	static DirectX::XMFLOAT4 dissolveColor = { 0,0,0,0 };
	if (colorChangeFlag)
	{
		static float i = 0;
		i += 1.0f * elapsed_time;
		tst.x = i;
		tst.y = i / 5;
		if (i > 1.5f)
		{
			pFadeOut.MoveStart();
		}
		titleTex->SetDissolveColor(dissolveColor);
		titleTex->SetDissolveThreshold(tst.x);
		titleTex->SetEmissiveWith(tst.y);

	}
	
#ifdef USE_IMGUI
	ImGui::Begin("Title");
	if (ImGui::Button("BGM")) soundManager->Play(static_cast<int>(SoundManager::SOUNDTITLE::ENTER), false);

	if (ImGui::Button("game_open"))
	{
		pFadeOut.MoveStart();
	}
	
	ImGui::InputFloat("dissolveThreshold", &tst.x, 0.1f);
	ImGui::InputFloat("emissiveWith", &tst.y, 0.1f);
	ImGui::ColorEdit3("dissolveColor", (float*)&dissolveColor);


	ImGui::End();
#endif

	return 0;

}

void SceneTitle::Render(float elapsed_time, ID3D11DeviceContext* devicecontext)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();

	
	
	blend->Activate(devicecontext);
#ifdef USE_IMGUI
	samplerWrap->Activate(devicecontext,0);
	rasterizer->SetRasterizerState(RS_CULL_BACK, devicecontext);

	//titleTex->DissolveRender(devicecontext, dissolveShader.get(), srv.Get(),0, 0, 1920, 1080, 0, 0, 1920, 1080, 0, 1, 1, 1, 1);
	titleTex->DissolveRender(devicecontext, dissolveShader.get(), srv.Get(), 308, 300, 1050, 169, 0, 0, 1050, 169, 0, 1, 1, 1, 1);
#else
	titleTex->DissolveRender(devicecontext, dissolveShader.get(),srv.Get(), 308, 300, 1050, 169, 0, 0, 1050, 169, 0, 1, 1, 1, 1);
#endif

	pFadeOut.Render(devicecontext);

	samplerWrap->Deactivate(devicecontext);
	blend->Deactivate(devicecontext);

}
SceneTitle::~SceneTitle()
{
	//pLoadModel.Destory();
}