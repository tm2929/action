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

bool SceneTitle::IsNowLoading()
{
	if (loading_thread && loading_mutex.try_lock())
	{
		loading_mutex.unlock();
		return false;
	}
	return true;
}

void SceneTitle::EndLoading()
{
	if (loading_thread && loading_thread->joinable())
	{
		loading_thread->join();
	}
}

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
			keyTex = std::make_unique<Sprite>(device, L"Data/images/STAET.png");
			buttonTex = std::make_unique<Sprite>(device, L"Data/images/button01.png");
			samplerWrap = std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
			rasterizer = std::make_unique<RasterizerState>();
			rasterizer->CreateRasterizerState(device);
			colorChangeFlag = false;
			dissolveShader = std::make_unique<ShaderEx>();
			D3D11_TEXTURE2D_DESC tex2D;
			load_texture_from_file(device, L"Data/dissolve/Shock_wave_ring001.png", srv.GetAddressOf(), &tex2D);
			dissolveShader->Create(device, "Data/shaders/cso/sprite_vs.cso", "Data/shaders/cso/Dissolve_ps.cso");
			frameBuffer[0] = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
			frameBuffer[1] = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
			bloomEffect = std::make_unique<Bloom>(device, 1920, 1080);
			//èëÇ´èoÇµ
			fullscreenQuad = std::make_unique<FullscreenQuad>(device);
			dissolveColor = DirectX::XMFLOAT4(0, 0.3f, 0.8f, 1);
			dissolveThreshold = 0;

			colorTimerSpeed = 2.0f;
			colorTimer = 0;
		}, device);
	//soundManager = std::make_unique<SoundManager>(hwnd);
	SoundManager::getinctance().Create(hwnd);
	SoundManager::getinctance().CreateSoundSourceTitle();
	SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDTITLE::BGM), true);
	blend = std::make_unique<BlendState>(device, BLEND_MODE::ALPHA);
}

int SceneTitle::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return 0;
	}
	EndLoading();

	if (FadeOut::GetInctence().Update(elapsed_time))
	{

		SoundManager::getinctance().Stop(static_cast<int>(SoundManager::SOUNDTITLE::BGM));
		return SceneName::GAME;
	}
	if (KeyInput::KeyTrigger() & KEY_ENTER || input::ButtonRisingState(0, input::PadLabel::A))
	{
		//FadeOut::GetInctence().MoveStart();
		SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDTITLE::ENTER), false);
		colorChangeFlag = true;
	}
	if (colorChangeFlag)
	{
		dissolveThreshold += elapsed_time * dissolveThresholdPlus;

		if (dissolveThreshold > sceneChangeTimer)
		{
			FadeOut::GetInctence().MoveStart();
			//dissolveThreshold = 0;
			//colorChangeFlag = false;
		}
		titleTex->SetDissolveColor(dissolveColor);
		titleTex->SetDissolveThreshold(dissolveThreshold);
		titleTex->SetEmissiveWidth(dissolveEmissiveWidth);

		keyTex->SetDissolveColor(dissolveColor);
		keyTex->SetDissolveThreshold(dissolveThreshold);
		keyTex->SetEmissiveWidth(dissolveEmissiveWidth);
		buttonTex->SetDissolveColor(dissolveColor);
		buttonTex->SetDissolveThreshold(dissolveThreshold);
		buttonTex->SetEmissiveWidth(dissolveEmissiveWidth);
	}

	colorTimer += elapsed_time * colorTimerSpeed;
	if (colorTimer <= colorTime[0])
	{
		texColorW = colorTimer;
	}
	else if (colorTimer <= colorTime[1])
	{
		texColorW = 1.0f;
	}
	else if (colorTimer <= colorTime[2])
	{
		texColorW = colorTime[2] - colorTimer;
	}
	else if (colorTimer <= colorTime[3])
	{
		texColorW = 0.0f;
	}
	if (colorTimer >= colorTime[3])
	{
		colorTimer = 0.0f;
	}
#ifdef USE_IMGUI
	ImGui::Begin("Title");
	if (ImGui::Button("")) SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDTITLE::ENTER), false);

	if (ImGui::Button("game_open"))
	{
		FadeOut::GetInctence().MoveStart();
	}

	ImGui::InputFloat("dissolveThreshold", &dissolveThreshold, 0.1f);
	ImGui::InputFloat("dissolveThresholdPlus", &dissolveThresholdPlus, 0.1f);
	ImGui::InputFloat("sceneChangeTimer", &sceneChangeTimer, 0.1f);
	ImGui::InputFloat("emissiveWith", &dissolveEmissiveWidth, 0.1f);
	ImGui::ColorEdit3("dissolveColor", (float*)&dissolveColor);

	float  blurConvolutionIntensity = bloomEffect->GetBlurConvolutionIntensity();
	ImGui::InputFloat(u8"ÇµÇ´Ç¢íl", &blurConvolutionIntensity, 0.1f);
	bloomEffect->SetBlurConvolutionIntensity(blurConvolutionIntensity);

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


	frameBuffer[0]->Clear(devicecontext);
	frameBuffer[0]->Activate(devicecontext);
	{
		blend->Activate(devicecontext);
#ifdef USE_IMGUI
		//samplerWrap->Activate(devicecontext, 0);
		//rasterizer->SetRasterizerState(RS_CULL_BACK, devicecontext);

		//titleTex->DissolveRender(devicecontext, dissolveShader.get(), srv.Get(),0, 0, 1920, 1080, 0, 0, 1920, 1080, 0, 1, 1, 1, 1);
		titleTex->DissolveRender(devicecontext, dissolveShader.get(), srv.Get(), 308, 300, 1050, 169, 0, 0, 1050, 169, 0, 1, 1, 1, 1);
#else
		titleTex->DissolveRender(devicecontext, dissolveShader.get(), srv.Get(), 308, 300, 1050, 169, 0, 0, 1050, 169, 0, 1, 1, 1, 1);
#endif
		keyTex->DissolveRender(devicecontext, dissolveShader.get(), srv.Get(), 300, 705, 346, 69, 0, 0, 346, 69, 0, 1, 1, 1, 0.8f);
		buttonTex->DissolveRender(devicecontext, dissolveShader.get(), srv.Get(), 700, 700, 84, 84, 0, 0, 84, 84, 0, 1, 1, 1, texColorW);
		FadeOut::GetInctence().Render(devicecontext);

		//samplerWrap->Deactivate(devicecontext);
		blend->Deactivate(devicecontext);
	}
	frameBuffer[0]->Deactivate(devicecontext);


	//ç≈èIâÊñ ï`âÊÅ@
	devicecontext->PSSetShaderResources(0, 1, frameBuffer[0]->GetRenderTargetShaderResourceView().GetAddressOf());
	fullscreenQuad->Blit(devicecontext, true, true, true);//ì]ëó

	bloomEffect->Generate(devicecontext, frameBuffer[0]->GetRenderTargetShaderResourceView().Get());
	bloomEffect->Blit(devicecontext);
}
SceneTitle::~SceneTitle()
{
	SoundManager::getinctance().Destroy();
	//pLoadModel.Destory();
}