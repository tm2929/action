#include "PlayerObj.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include "VectorCombo.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif
PlayerObj::PlayerObj(std::shared_ptr<ModelResource> resource)
	:Character(resource)
{

	LoadBoneNum();
}

void PlayerObj::BoneNameInput()
{
	for (auto& m : GetModel()->GetNodes())
	{
		boneName.emplace_back(m.name);
	}
}

void PlayerObj::SaveBoneNum()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/player_obj_boon_num.bin", "wb");
	fwrite(&attackBoneNum, sizeof(int), 1, fp);
	fclose(fp);
}

int PlayerObj::LoadBoneNum()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/player_obj_boon_num.bin", "rb") == 0)
	{
		fread(&attackBoneNum, sizeof(int), 1, fp);
		fclose(fp);
	}
	return attackBoneNum;
}

void PlayerObj::AttackBoneNameEditor()
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
		ImGui::Text("boneNum%d", n);
		if (ImGui::Button("save"))
		{
			SaveBoneNum();
		}
		if (ImGui::Button("load"))
		{
			n = LoadBoneNum();
		}
		ImGui::InputFloat("pulsx", &pulsPos.x, 10.f);
		ImGui::InputFloat("pulsy", &pulsPos.y, 1.f);
		ImGui::InputFloat("pulsz", &pulsPos.z, 10.f);
	}


#endif
}

void PlayerObj::HitAttackTransform()
{
	char* s = { "joint40" };
	for (auto& bone : GetModel()->GetNodes())
	{
		//•¶Žš—ñ‚ð”äŠr‚·‚éê‡ strcmp[a,b]
		if (strcmp(bone.name, s) == 0)
		{

			hitSphere.position.x = bone.worldTransform._41; //+ ((bone.worldTransform._31) * tstPos.x);
			hitSphere.position.y = bone.worldTransform._42; //+ ((bone.worldTransform._32) * tstPos.y);
			hitSphere.position.z = bone.worldTransform._43; //+ ((bone.worldTransform._33) * tstPos.z);
		}
	}
	DirectX::XMFLOAT3 position = GetPosition();
	CalculateTransform();

#ifdef USE_IMGUI
	/*ImGui::Begin("hone");

	ImGui::SliderInt("pos x", &tstPos.x, -100, 50);
	ImGui::SliderInt("pos y", &tstPos.y, -300, 50);
	ImGui::SliderInt("pos z", &tstPos.z, -100, 50);

	ImGui::End();*/
#endif
}

void PlayerObj::HitAttackTransformIMGUI()
{
	Model::Node attackNode = GetModel()->GetNodes()[attackBoneNum];

	hitSphere.position.x = attackNode.worldTransform._41 + ((attackNode.worldTransform._31) * pulsPos.x);
	hitSphere.position.y = attackNode.worldTransform._42 + ((attackNode.worldTransform._32) * pulsPos.y);
	hitSphere.position.z = attackNode.worldTransform._43 + ((attackNode.worldTransform._33) * pulsPos.z);
}
