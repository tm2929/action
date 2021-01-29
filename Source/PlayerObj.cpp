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
	LoadKickBoneNum();
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
void  PlayerObj::SaveKickBoneNum()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/player_obj_boon_kick_num.bin", "wb");
	fwrite(&kickBoneNum, sizeof(int), 1, fp);
	fclose(fp);
}
int PlayerObj::LoadKickBoneNum()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/player_obj_boon_kick_num.bin", "rb") == 0)
	{
		fread(&kickBoneNum, sizeof(int), 1, fp);
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
			kickBoneNum = n;
		}
		ImGui::Text("boneNum%d", n);
		if (ImGui::Button("save"))
		{
			SaveBoneNum();
		}
		if (ImGui::Button("kickSave"))
		{
			SaveKickBoneNum();
		}
		if (ImGui::Button("load"))
		{
			n = LoadBoneNum();
		}
		if (ImGui::Button("kickLoad"))
		{
			n = LoadKickBoneNum();
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

void PlayerObj::HitAttackTransformIMGUI(bool kickFlag )
{
	Model::Node node;
	if (kickFlag)
	{
		 node = GetModel()->GetNodes()[kickBoneNum];
		 pulsPos = { 0,0,0 };
	}
	else
	{
		 node = GetModel()->GetNodes()[attackBoneNum];
		 pulsPos = { 110,100,110 };
	}


	hitSphere.position.x = node.worldTransform._41 + ((node.worldTransform._31) * pulsPos.x);
	hitSphere.position.y = node.worldTransform._42 + ((node.worldTransform._32) * pulsPos.y);
	hitSphere.position.z = node.worldTransform._43 + ((node.worldTransform._33) * pulsPos.z);
}
