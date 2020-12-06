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

SceneTitle::SceneTitle(ID3D11Device* device, HWND hwnd)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);
#ifdef USE_IMGUI
			titleTex = std::make_unique<Sprite>(device, L"Data/images/bg.png");
#else
			titleTex = std::make_unique<Sprite>(device, L"Data/images/Title01.png");
#endif
			colorChangeFlag = false;
		}, device);

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
		return SceneName::GAME;
	}
	if (KeyInput::KeyTrigger() & KEY_ENTER || input::ButtonRisingState(0, input::PadLabel::A))
	{
		pFadeOut.MoveStart();
		colorChangeFlag = true;
	}
	if (colorChangeFlag)
	{
		static float dispTimer, dispMaxTimer;
		dispMaxTimer = 0.5f;

	}
#ifdef USE_IMGUI
	ImGui::Begin("Title");

	if (ImGui::Button("game_open"))
	{
		pFadeOut.MoveStart();
	}

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
	titleTex->Render(devicecontext, 0, 0, 1920, 1080, 0, 0, 1920, 1080, 0, 1, 1, 1, 1);
#else
	titleTex->Render(devicecontext, 308, 300, 1050, 169, 0, 0, 1050, 169, 0, 1, 1, 1, 1);
#endif
	pFadeOut.Render(devicecontext);

	blend->Deactivate(devicecontext);

}
SceneTitle::~SceneTitle()
{
	//pLoadModel.Destory();
}