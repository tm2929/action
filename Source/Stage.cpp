#include "Stage.h"
#include "LoadModel.h"
//#include <iostream>
//#include <sstream>
#include "HitAreaRnder.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif
Stage::Stage(ID3D11Device* device)
{
	pos = { 245,-100,-280 };
	//pos = { 0,0,0 };
	max = { 60,400,500 };
	//max = { 30,10,30};
	//min = { 30,10,30};
	pLoadModel.Load(device, "Data/fbx/tstStage/3/tstLeft1.fbx", "tstLeft1");
	pLoadModel.Load(device, "Data/fbx/tstStage/3/tstLeft2.fbx", "tstLeft2");
	pLoadModel.Load(device, "Data/fbx/tstStage/3/tstRight1.fbx", "tstRight1");
	pLoadModel.Load(device, "Data/fbx/tstStage/3/tstRight2.fbx", "tstRight2");
	pLoadModel.Load(device, "Data/fbx/tstStage/3/tstFront1.fbx", "tstFront1");
	pLoadModel.Load(device, "Data/fbx/tstStage/3/tstFront2.fbx", "tstFront2");
	pLoadModel.Load(device, "Data/fbx/tstStage/3/haka.fbx", "haka");
	pLoadModel.Load(device, "Data/fbx/tstStage/3/back1.fbx", "tstBack1");
	pLoadModel.Load(device, "Data/fbx/tstStage/3/back2.fbx", "tstBack2");
	pLoadModel.Load(device, "Data/fbx/tstStage/3/iriguti.fbx", "iriguti");
	pLoadModel.Load(device, "Data/fbx/tstStage/3/floor2.fbx", "Floor");
	pLoadModel.Load(device, "Data/fbx/tstStage/3/stage02.fbx", "Stage");
	pLoadModel.Load(device, "Data/fbx/tstStage/3/hitLeft.fbx", "hitLeft");
	tstLeft1 = std::make_shared<Character>(pLoadModel.GetModelResource("tstLeft1"));
	tstLeft1->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	tstLeft1->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	tstLeft1->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	tstLeft2 = std::make_shared<Character>(pLoadModel.GetModelResource("tstLeft2"));
	tstLeft2->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	tstLeft2->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	tstLeft2->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	tstRight1 = std::make_shared<Character>(pLoadModel.GetModelResource("tstRight1"));
	tstRight1->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	tstRight1->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	tstRight1->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	tstRight2 = std::make_shared<Character>(pLoadModel.GetModelResource("tstRight2"));
	tstRight2->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	tstRight2->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	tstRight2->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	tstFront2 = std::make_shared<Character>(pLoadModel.GetModelResource("tstFront1"));
	tstFront2->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	tstFront2->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	tstFront2->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	tstFront1 = std::make_shared<Character>(pLoadModel.GetModelResource("tstFront2"));
	tstFront1->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	tstFront1->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	tstFront1->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	tstBack2 = std::make_shared<Character>(pLoadModel.GetModelResource("tstBack1"));
	tstBack2->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	tstBack2->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	tstBack2->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	tstBack1 = std::make_shared<Character>(pLoadModel.GetModelResource("tstBack2"));
	tstBack1->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	tstBack1->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	tstBack1->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	haka1 = std::make_shared<Character>(pLoadModel.GetModelResource("haka"));
	haka1->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	haka1->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	haka1->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	haka2 = std::make_shared<Character>(pLoadModel.GetModelResource("haka"));
	haka2->SetPosition(DirectX::XMFLOAT3(-3.0f, 0.0f, -5.0f));
	haka2->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	haka2->SetAngle(DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f));

	floor = std::make_shared<Obj3D>(pLoadModel.GetModelResource("Floor"));
	floor->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, .0f));
	floor->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	floor->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	left1Cube.max = max;
	left1Cube.min = min;

}

void Stage::Update(float elapsedTime)
{
	tstLeft1->CalculateTransform();
	tstLeft2->CalculateTransform();
	tstRight1->CalculateTransform();
	tstRight2->CalculateTransform();
	tstFront1->CalculateTransform();
	tstFront2->CalculateTransform();
	tstBack1->CalculateTransform();
	haka1->CalculateTransform();
	haka2->CalculateTransform();
	tstBack2->CalculateTransform();
	//hitLeft->CalculateTransform();
	floor->CalculateTransform();
	left1Cube.HitAreaTransform({ 261,0,0 }, { 0,0,0 }, { 100,60,255 });
	//left1Cube.HitAreaTransform({ pos}, { 0,0,0 }, { max });
	left2Cube.HitAreaTransform({ 261,0,-255 }, { 0,0,0 }, { 100,60,255 });
	right1Cube.HitAreaTransform({ -361,0,0 }, { 0,0,0 }, { 100,60,255 });
	right2Cube.HitAreaTransform({ -361,0,-255 }, { 0,0,0 }, { 100,60,255 });
	front1Cube.HitAreaTransform({ 67,0,-336 }, { 0,0,0 }, { 200,60,100 });
	front2Cube.HitAreaTransform({ -262,0,-336 }, { 0,0,0 }, { 181,60,100 });
	haka1Cube.HitAreaTransform({ -81,0,-336 }, { 0,0,0 }, { 148,60,100, });
	back1Cube.HitAreaTransform({ 67,0,229 }, { 0,0,0 }, { 190,60,100 });
	back2Cube.HitAreaTransform({ -259,0,228 }, { 0,0,0 }, { 184,60,100 });
	haka2Cube.HitAreaTransform({ -75,0,231 }, { 0,0,0 }, { 142,60,100 });

	pHitAreaRender.SetHitCube(left1Cube.min, left1Cube.max, DirectX::XMFLOAT4(1, 1, 1, 0.3f));
	pHitAreaRender.SetHitCube(left2Cube.min, left2Cube.max, DirectX::XMFLOAT4(1, 1, 1, 0.3f));
	pHitAreaRender.SetHitCube(right1Cube.min, right1Cube.max, DirectX::XMFLOAT4(1, 1, 1, 0.3f));
	pHitAreaRender.SetHitCube(right2Cube.min, right2Cube.max, DirectX::XMFLOAT4(1, 1, 1, 0.3f));
	pHitAreaRender.SetHitCube(front1Cube.min, front1Cube.max, DirectX::XMFLOAT4(1, 1, 1, 0.3f));
	pHitAreaRender.SetHitCube(front2Cube.min, front2Cube.max, DirectX::XMFLOAT4(1, 1, 1, 0.3f));
	pHitAreaRender.SetHitCube(haka1Cube.min, haka1Cube.max, DirectX::XMFLOAT4(1, 1, 1, 0.3f));
	pHitAreaRender.SetHitCube(back1Cube.min, back1Cube.max, DirectX::XMFLOAT4(1, 1, 1, 0.3f));
	pHitAreaRender.SetHitCube(haka2Cube.min, haka2Cube.max, DirectX::XMFLOAT4(1, 1, 1, 0.3f));
}
void Stage::Imgui()
{
#ifdef USE_IMGUI
	ImGui::InputFloat("max_x", &max.x, 0.1f);
	ImGui::InputFloat("max_y", &max.y, 0.1f);
	ImGui::InputFloat("max_z", &max.z, 0.1f);

	ImGui::InputFloat("min_x", &min.x, 0.1f);
	ImGui::InputFloat("min_y", &min.y, 0.1f);
	ImGui::InputFloat("min_z", &min.z, 0.1f);

	ImGui::InputFloat("pos_x", &pos.x, 1.f);
	ImGui::InputFloat("pos_y", &pos.y, 1.f);
	ImGui::InputFloat("pos_z", &pos.z, 1.f);
#endif
}
