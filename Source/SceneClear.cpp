#include "SceneClear.h"
#include "Fadeout.h"
#include "KeyInput.h"
#include "gamepad.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif
#include "Sound.h"
#include "Texture.h"

bool SceneClear::IsNowLoading()
{
	if (loading_thread && loading_mutex.try_lock())
	{
		loading_mutex.unlock();
		return false;
	}
	return true;
}

void SceneClear::EndLoading()
{
	if (loading_thread && loading_thread->joinable())
	{
		loading_thread->join();
	}
}

SceneClear::SceneClear(ID3D11Device* device, HWND hwnd)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);
			clearTex = std::make_unique<Sprite>(device, L"Data/images/CLEAR.png");
			retryTex = std::make_unique<Sprite>(device, L"Data/images/RETRY.png");
			titleTex = std::make_unique<Sprite>(device, L"Data/images/Title01.png");
			choiceTex = std::make_unique<Sprite>(device, L"Data/images/arrow..png");
			dissolveFlag = false;
			dissolveShader = std::make_unique<ShaderEx>();
			D3D11_TEXTURE2D_DESC tex2D;
			load_texture_from_file(device, L"Data/dissolve/Shock_wave_ring001.png", srv.GetAddressOf(), &tex2D);
			dissolveShader->Create(device, "Data/shaders/cso/sprite_vs.cso", "Data/shaders/cso/Dissolve_ps.cso");
			frameBuffer[0] = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
			frameBuffer[1] = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
			bloomEffect = std::make_unique<Bloom>(device, 1920, 1080);
			//èëÇ´èoÇµ
			fullscreenQuad = std::make_unique<FullscreenQuad>(device);
			dissolveColor = DirectX::XMFLOAT4(0,0.3f, 0.8f,1);
			dissolveThreshold = 0;

			colorTimerSpeed = 2.0f;
			choicePos = { 222,585 };
			retryFlag = false;
			colorTimer = 0;
		}, device);
	nowLoadingTex = std::make_unique<Sprite>(device, L"Data/images/NOW_LOADING.png");
	SoundManager::getinctance().Create(hwnd);
	SoundManager::getinctance().CreateSoundSourceTitle();
	SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDTITLE::BGM), true);
	blend = std::make_unique<BlendState>(device, BLEND_MODE::ALPHA);

}

int SceneClear::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return 0;
	}
	EndLoading();

	float power_x = .0f;
	power_x = input::StickStateX(0, false);
	if (retryFlag)
	{
		if (power_x < -0.3)
		{
			choicePos.x = titlePos;
		}
		else if (input::ButtonRisingState(0, input::PadLabel::LEFT) || (KeyInput::KeyState() & KEY_LEFT))
		{
			choicePos.x = titlePos;
			SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDTITLE::SELECT), false);
		}
	}
	else
	{
		if (power_x > 0.3)
		{
			choicePos.x = retryPos;
		}
		else if (input::ButtonRisingState(0, input::PadLabel::RIGHT) || (KeyInput::KeyState() & KEY_RIGHT))
		{
			choicePos.x = retryPos;
			SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDTITLE::SELECT), false);
		}
	}
	if (choicePos.x == retryPos)
	{
		retryFlag = true;
	}
	else
	{
		retryFlag = false;
	}
	if (input::ButtonRisingState(0, input::PadLabel::A)|| (KeyInput::KeyState() & KEY_ENTER))
	{
		//FadeOut::GetInctence().MoveStart();
		SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDTITLE::ENTER), false);
		dissolveFlag = true;
	}
	if (dissolveFlag)
	{
		dissolveThreshold += elapsed_time * dissolveThresholdPlus;

		if (dissolveThreshold > sceneChangeTimer)
		{
			FadeOut::GetInctence().MoveStart();	
		}
		titleTex->SetDissolveColor(dissolveColor);
		titleTex->SetDissolveThreshold(dissolveThreshold);
		titleTex->SetEmissiveWidth(dissolveEmissiveWidth);

		retryTex->SetDissolveColor(dissolveColor);
		retryTex->SetDissolveThreshold(dissolveThreshold);
		retryTex->SetEmissiveWidth(dissolveEmissiveWidth);

		clearTex->SetDissolveColor(dissolveColor);
		clearTex->SetDissolveThreshold(dissolveThreshold);
		clearTex->SetEmissiveWidth(dissolveEmissiveWidth);
		
		choiceTex->SetDissolveColor(dissolveColor);
		choiceTex->SetDissolveThreshold(dissolveThreshold);
		choiceTex->SetEmissiveWidth(dissolveEmissiveWidth);

		
	}
	if (FadeOut::GetInctence().Update(elapsed_time))
	{
		SoundManager::getinctance().Stop(static_cast<int>(SoundManager::SOUNDTITLE::BGM));
		if (retryFlag)
		{
			return SceneName::GAME;
		}
		else
		{
			return SceneName::TITLE;
		}
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
	ImGui::Begin("Clear");

	if (ImGui::Button("Ttitle_open"))
	{
		FadeOut::GetInctence().MoveStart();
	}

	ImGui::End();
#endif

	return 0;
}

void SceneClear::Render(float elapsed_time, ID3D11DeviceContext* devicecontext)
{
	if (IsNowLoading())
	{
		nowLoadingTex->Render(devicecontext, 50, 1000, 416, 32, 0, 0, 832, 64, 0, 1, 1, 1, 1);
		return;
	}
	EndLoading();

	frameBuffer[0]->Clear(devicecontext);
	frameBuffer[0]->Activate(devicecontext);
	{
		blend->Activate(devicecontext);

		retryTex->DissolveRender(devicecontext, dissolveShader.get(), srv.Get(), 1224, 585, 344, 65, 0, 0, 344, 65, 0, 1, 1, 1, 1);
		titleTex->DissolveRender(devicecontext, dissolveShader.get(), srv.Get(), 300, 585, 346, 69, 0, 0, 1050, 169, 0, 1, 1, 1, 1);
		clearTex->DissolveRender(devicecontext, dissolveShader.get(), srv.Get(), 602, 300, 716, 136, 0, 0, 716, 136, 0, 1, 1, 1, 1);
		choiceTex->DissolveRender(devicecontext, dissolveShader.get(), srv.Get(), choicePos.x, choicePos.y, 68, 79, 0, 0, 68, 79, 0, 1, 1, 1, texColorW);
		FadeOut::GetInctence().Render(devicecontext);

		blend->Deactivate(devicecontext);
	}
	frameBuffer[0]->Deactivate(devicecontext);
	//ç≈èIâÊñ ï`âÊÅ@
	devicecontext->PSSetShaderResources(0, 1, frameBuffer[0]->GetRenderTargetShaderResourceView().GetAddressOf());
	fullscreenQuad->Blit(devicecontext, true, true, true);//ì]ëó

	bloomEffect->Generate(devicecontext, frameBuffer[0]->GetRenderTargetShaderResourceView().Get());
	bloomEffect->Blit(devicecontext);
}

SceneClear::~SceneClear()
{
	SoundManager::getinctance().Destroy();
}
