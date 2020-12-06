#include "EnemyObj.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include"vectorCombo.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif
EnemyObj::EnemyObj(std::shared_ptr<ModelResource> resource)
	:Character(resource)
{
	LoadBoneNum();
}

void EnemyObj::BoneNameInput()
{
	for (auto& m : GetModel()->GetNodes())
	{
		boneName.emplace_back(m.name);
	}
}

void EnemyObj::SaveBoneNum()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/enemy_obj_boon_num.bin", "wb");
	fwrite(&attackBoneNum, sizeof(int), 1, fp);
	fclose(fp);
}
void EnemyObj::LoadBoneNum()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/enemy_obj_boon_num.bin", "rb") == 0)
	{
		fread(&attackBoneNum, sizeof(int), 1, fp);
		fclose(fp);
	}
}
void EnemyObj::AttackBoneNameEditor()
{
#ifdef USE_IMGUI
	static int n = 0;

	if (ImGui::CollapsingHeader("bone"))
	{
		ImGui::Combo("bone_Name", &n, vector_getter, static_cast<void*>(&boneName), static_cast<int>(boneName.size()));
		if (ImGui::Button("set"))
		{
			attackBoneNum = n;
		}
		if (ImGui::Button("save"))
		{
			SaveBoneNum();
		}
		ImGui::SliderFloat("pulsx", &pulsPos.x, -150, 150.f);
		ImGui::SliderFloat("pulsy", &pulsPos.y, -30, 0.f);
		ImGui::SliderFloat("pulsz", &pulsPos.z, -150, 150.f);
	}


#endif
}
void EnemyObj::HitAttackTransform()
{
	Model::Node attackNode = GetModel()->GetNodes()[attackBoneNum];

	hitSphere.position.x = attackNode.worldTransform._41 + ((attackNode.worldTransform._31) * pulsPos.x);
	hitSphere.position.y = attackNode.worldTransform._42 + pulsPos.y;
	hitSphere.position.z = attackNode.worldTransform._43 + ((attackNode.worldTransform._33) * pulsPos.z);
}
