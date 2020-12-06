
#include "sky.h"
#include "character_manager.h"
#include "enemy.h"

#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

SkyManager* SkyManager::instance = nullptr;

void Sky::Move(Obj3D* obj, float elapsedTime)
{

}

void SkyManager::Init(ID3D11Device* device)
{
	Obj3DManager::Init(device);
}

void SkyManager::Update(float elapsedTime)
{
	Obj3DManager::Update(elapsedTime);
}
