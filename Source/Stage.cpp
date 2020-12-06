#include "stage.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

Stage::Stage(ID3D11Device* device)
{
	const char* fbx_filename3 = "Data/FBX/BG/floor01_c.fbx";
	std::shared_ptr<ModelResource> model_resource3 = std::make_shared<ModelResource>(device, fbx_filename3);
	floor = std::make_shared<Obj3D>(model_resource3);

	const char* fbx_filename4 = "Data/FBX/BG/wall01_c.fbx";
	std::shared_ptr<ModelResource> model_resource4 = std::make_shared<ModelResource>(device, fbx_filename4);
	for (int i = 0; i < 4; i++)
	{
		wall[i] = std::make_shared<Obj3D>(model_resource4);
	}
	Load();
	wall[3]->SetAngle(DirectX::XMFLOAT3(0, 3.14f * 1.5f, 0));
}

void Stage::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("state");
	if (ImGui::CollapsingHeader("floor"))
	{
		Data floorData;
		floorData.position = floor->GetPosition();
		floorData.scale = floor->GetScale();
		floorData.angle = floor->GetAngle();

		ImGui::DragFloat("floorPosition.x", &floorData.position.x);
		ImGui::DragFloat("floorPosition.y", &floorData.position.y);
		ImGui::DragFloat("floorPosition.z", &floorData.position.z);

		ImGui::DragFloat("floorScale.x", &floorData.scale.x);
		ImGui::DragFloat("floorScale.y", &floorData.scale.y);
		ImGui::DragFloat("floorScale.z", &floorData.scale.z);

		ImGui::SliderFloat("floorAngle.x", &floorData.angle.x, 0, 3.14f * 2.f);
		ImGui::SliderFloat("floorAngle.y", &floorData.angle.y, 0, 3.14f * 2.f);
		ImGui::SliderFloat("floorAngle.z", &floorData.angle.z, 0, 3.14f * 2.f);

		floor->SetPosition(floorData.position);
		floor->SetScale(floorData.scale);
		floor->SetAngle(floorData.angle);

	}
	if (ImGui::CollapsingHeader("wall"))
	{
		static int wallNum = 0;
		ImGui::SliderInt("wall num", &wallNum, 0, 3);
		Data wallData;
		wallData.position = wall[wallNum]->GetPosition();
		wallData.scale = wall[wallNum]->GetScale();
		wallData.angle = wall[wallNum]->GetAngle();

		ImGui::DragFloat("wallPosition.x", &wallData.position.x);
		ImGui::DragFloat("wallPosition.y", &wallData.position.y);
		ImGui::DragFloat("wallPosition.z", &wallData.position.z);

		ImGui::DragFloat("wallScale.x", &wallData.scale.x);
		ImGui::DragFloat("wallScale.y", &wallData.scale.y);
		ImGui::DragFloat("wallScale.z", &wallData.scale.z);

		ImGui::SliderFloat("wallAngle.x", &wallData.angle.x, 0, 3.14f * 2.f);
		ImGui::SliderAngle("wallAngle.y", &wallData.angle.y);
		ImGui::SliderFloat("wallAngle.z", &wallData.angle.z, 0, 3.14f * 2.f);

		wall[wallNum]->SetPosition(wallData.position);
		wall[wallNum]->SetScale(wallData.scale);
		wall[wallNum]->SetAngle(wallData.angle);

	}
	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::End();
	floor->CalculateTransform();
	for (int i = 0; i < 4; i++)
	{
		wall[i]->CalculateTransform();
	}
#endif
}

void Stage::Update()
{
	floor->CalculateTransform();
	for (int i = 0; i < 4; i++)
	{
		wall[i]->CalculateTransform();
	}
}

void Stage::Load()
{
	Data floorData;
	FILE* fp;
	if (fopen_s(&fp, "Data/file/floor.bin", "rb") == 0)
	{
		fread(&floorData, sizeof(Data), 1, fp);
		fclose(fp);
	}
	floor->SetPosition(floorData.position);
	floor->SetScale(floorData.scale);
	floor->SetAngle(floorData.angle);
	Data wallData[4];
	if (fopen_s(&fp, "Data/file/wall.bin", "rb") == 0)
	{

		fread(&wallData[0], sizeof(Data), 4, fp);
		fclose(fp);
	}
	for (int i = 0; i < 4; i++)
	{
		wall[i]->SetPosition(wallData[i].position);
		wall[i]->SetScale(wallData[i].scale);
		wall[i]->SetAngle(wallData[i].angle);
	}
}

void Stage::Save()
{
	Data floorData;
	FILE* fp;
	floorData.position = floor->GetPosition();
	floorData.scale = floor->GetScale();
	floorData.angle = floor->GetAngle();

	fopen_s(&fp, "Data/file/floor.bin", "wb");
	fwrite(&floorData, sizeof(Data), 1, fp);
	fclose(fp);

	Data wallData[4];
	for (int i = 0; i < 4; i++)
	{
		wallData[i].position = wall[i]->GetPosition();
		wallData[i].scale = wall[i]->GetScale();
		wallData[i].angle = wall[i]->GetAngle();
	}

	fopen_s(&fp, "Data/file/wall.bin", "wb");
	fwrite(&wallData[0], sizeof(Data), 4, fp);
	fclose(fp);

}
