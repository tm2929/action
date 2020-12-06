#pragma once
#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <memory>

#include <wrl.h>
#include "HighResolutionTimer.h"

#include <d3d11.h>
#include <stdlib.h>

#include "SceneManager.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

const LONG SCREEN_WIDTH = 1920;
const LONG SCREEN_HEIGHT = 1080;

class framework
{
public:
	const HWND hwnd;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;

	Microsoft::WRL::ComPtr<IDXGISwapChain>		   swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView ;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView ;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>		   texture2d ;
	

	framework(HWND hwnd) : hwnd(hwnd)
	{

	}
	~framework()
	{
		if (device) {
			device->Release();
		}
		if (deviceContext) {
			deviceContext->Release();
		}

		pSceneManager.Destroy();
	}
	int run()
	{
		MSG msg = {};

		if (!initialize(hwnd)) return 0;
		timer.reset();

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{

				timer.tick();
				calculate_frame_stats();
				update(timer.time_interval());
				render(timer.time_interval());

			}
		}
		//delete[] *sprites;
#ifdef USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif


		return static_cast<int>(msg.wParam);
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return 1;
#endif

		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_ENTERSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			timer.stop();
			break;
		case WM_EXITSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
			timer.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}

		return 0;
	}

private:
	bool initialize(HWND hwnd);
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);

private:
	//FPS
	high_resolution_timer timer;
	void calculate_frame_stats()
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.
		static int frames = 0;
		static float time_tlapsed = 0.0f;

		frames++;

		// Compute averages over one second period.
		if ((timer.time_stamp() - time_tlapsed) >= 1.0f)
		{
			float fps = static_cast<float>(frames); // fps = frameCnt / 1
			float mspf = 1000.0f / fps;
			std::ostringstream outs;
			outs.precision(6);
			outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
			SetWindowTextA(hwnd, outs.str().c_str());

			// Reset for next average.
			frames = 0;
			time_tlapsed += 1.0f;
		}
	}
};

