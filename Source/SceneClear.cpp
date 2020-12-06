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

SceneClear::SceneClear(ID3D11Device* device, HWND hwnd)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);
			clear = std::make_unique<Sprite>(device, L"Data/images/CLEAR.png");
			retry = std::make_unique<Sprite>(device, L"Data/images/RETRY.png");
			title = std::make_unique<Sprite>(device, L"Data/images/STAET.png");
			choice = std::make_unique<Sprite>(device, L"Data/images/arrow..png");

			choicePos = { 222,585 };
			retryPos = { 0,0 };
			titlePos = { 0,0 };
			retryFlag = false;
			colorTimer = 0;
		}, device);
	nowLoading = std::make_unique<Sprite>(device, L"Data/images/NOW_LOADING.png");
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
			choicePos.x = 222;
		}
		else if (input::ButtonRisingState(0, input::PadLabel::LEFT) || (KeyInput::KeyState() & KEY_LEFT))
		{
			choicePos.x = 222;
		}
	}
	else
	{
		if (power_x > 0.3)
		{
			choicePos.x = 1146;
		}
		else if (input::ButtonRisingState(0, input::PadLabel::RIGHT) || (KeyInput::KeyState() & KEY_RIGHT))
		{
			choicePos.x = 1146;
		}
	}
	if (choicePos.x == 1146)
	{
		retryFlag = true;
	}
	else
	{
		retryFlag = false;
	}
	if (input::ButtonRisingState(0, input::PadLabel::A)|| (KeyInput::KeyState() & KEY_ENTER))
	{
		pFadeOut.MoveStart();
	}
	if (pFadeOut.Update(elapsed_time))
	{
		if (retryFlag)
		{
			return SceneName::GAME;
		}
		else
		{
			return SceneName::TITLE;
		}
	}
	colorTimer += elapsed_time * 2.0f;
	if (colorTimer <= 1)
	{
		texColorW = colorTimer;
	}
	else if (colorTimer <= 1.5f)
	{
		texColorW = 1.0f;
	}
	else if (colorTimer <= 2.5f)
	{
		texColorW = 2.5f - colorTimer;
	}
	else if (colorTimer <= 3.0f)
	{
		texColorW = 0.0f;
	}
	if (colorTimer >= 3.0f)
	{
		colorTimer = 0.0f;
	}
#ifdef USE_IMGUI
	ImGui::Begin("Clear");

	if (ImGui::Button("Ttitle_open"))
	{
		pFadeOut.MoveStart();
	}

	ImGui::End();
#endif

	return 0;
}

void SceneClear::Render(float elapsed_time, ID3D11DeviceContext* devicecontext)
{
	if (IsNowLoading())
	{
		nowLoading->Render(devicecontext, 50, 1000, 416, 32, 0, 0, 832, 64, 0, 1, 1, 1, 1);
		return;
	}
	EndLoading();

	blend->Activate(devicecontext);

	retry->Render(devicecontext, 1224, 585, 344, 65, 0, 0, 344, 65, 0, 1, 1, 1, 1);
	title->Render(devicecontext, 300, 585, 346, 69, 0, 0, 346, 69, 0, 1, 1, 1, 1);
	clear->Render(devicecontext, 602, 300, 716, 136, 0, 0, 716, 136, 0, 1, 1, 1, 1);
	choice->Render(devicecontext, choicePos.x, choicePos.y, 68, 79, 0, 0, 68, 79, 0, 1, 1, 1, texColorW);
	pFadeOut.Render(devicecontext);

	blend->Deactivate(devicecontext);
}

SceneClear::~SceneClear()
{
}
