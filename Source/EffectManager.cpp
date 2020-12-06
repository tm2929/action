#include "EffectManager.h"
#include "Camera.h"
#include<assert.h>
#include <wrl\client.h>
void EffectObj::Init(ID3D11Device* device)
{
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context;
	device->GetImmediateContext(immediate_context.GetAddressOf());
	renderer = EffekseerRendererDX11::Renderer::Create(device, immediate_context.Get(), 1000);
	manager = ::Effekseer::Manager::Create(1000);

	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	manager->SetTrackRenderer(renderer->CreateTrackRenderer());
	manager->SetModelRenderer(renderer->CreateModelRenderer());
	manager->SetTextureLoader(renderer->CreateTextureLoader());
	manager->SetModelLoader(renderer->CreateModelLoader());
	manager->SetMaterialLoader(renderer->CreateMaterialLoader());

	manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	effect[static_cast<int>(TYPE::THUNDER)] = Effekseer::Effect::Create(manager, (EFK_CHAR*)L"Data/Effect/THUNDER.efk");
	effect[static_cast<int>(TYPE::BLACKMIST)] = Effekseer::Effect::Create(manager, (EFK_CHAR*)L"Data/Effect/BLACKMIST.efk");
	effect[static_cast<int>(TYPE::TST)] = Effekseer::Effect::Create(manager, (EFK_CHAR*)L"Data/Effect/run.efk");
	effect[static_cast<int>(TYPE::CHARGE)] = Effekseer::Effect::Create(manager, (EFK_CHAR*)L"Data/Effect/CHARGE.efk");
	effect[static_cast<int>(TYPE::SLASHING)] = Effekseer::Effect::Create(manager, (EFK_CHAR*)L"Data/Effect/huriage.efk");
	effect[static_cast<int>(TYPE::BRIBIRI)] = Effekseer::Effect::Create(manager, (EFK_CHAR*)L"Data/Effect/biribiri.efk");
	//e = Effekseer::Effect::Create(manager, (EFK_CHAR*)L"Pierre01/tst.efk");

	for (int i = 0; i < num; i++)
	{
		pos[i] = DirectX::XMFLOAT3(0, 0, 0);
		scale[i] = DirectX::XMFLOAT3(1, 1, 1);
		angle[i] = DirectX::XMFLOAT3(0, 0, 0);
		color[i] = Effekseer::Color(255, 255, 255, 255);
		stopFlag[i] = false;
		straightFlag = false;
		manager->SetLocation(handle[i], pos[i].x, pos[i].y, pos[i].z);//位置
		manager->SetRotation(handle[i], angle[i].x, angle[i].y, angle[i].z);//回転
		manager->SetScale(handle[i], scale[i].x, scale[i].y, scale[i].z);//倍率
		manager->SetAllColor(handle[i], color[i]);
	}
	count = 0;

}

void EffectObj::Update()
{
	renderer->SetProjectionMatrix(Effekseer::Matrix44().PerspectiveFovLH(Camera::GetInstance().FoV * (3.14f / 180.f), Camera::GetInstance().aspect, Camera::GetInstance().nearZ, Camera::GetInstance().farZ));
	renderer->SetCameraMatrix(Effekseer::Matrix44().LookAtLH(Effekseer::Vector3D(Camera::GetInstance().GetEye().x, Camera::GetInstance().GetEye().y, Camera::GetInstance().GetEye().z), Effekseer::Vector3D(Camera::GetInstance().GetFocus().x, Camera::GetInstance().GetFocus().y, Camera::GetInstance().GetFocus().z), Effekseer::Vector3D(Camera::GetInstance().GetUp().x, Camera::GetInstance().GetUp().y, Camera::GetInstance().GetUp().z)));
	for (int i = 0; i < num; i++)
	{
		manager->SetScale(handle[i], scale[i].x, scale[i].y, scale[i].z);//倍率
		manager->SetLocation(handle[i], pos[i].x, pos[i].y, pos[i].z);//位置
		manager->SetRotation(handle[i], angle[i].x, angle[i].y, angle[i].z);
		manager->SetAllColor(handle[i], color[i]);
	}
	manager->Update();
}
void EffectObj::Render()
{
	renderer->BeginRendering();//エフェクトの描画開始処理
	manager->Draw();//エフェクトの描画
	renderer->EndRendering();//エフェクトの描画終了処理
}

void EffectObj::Play(TYPE i)
{
	handle[static_cast<int>(i)] = manager->Play(effect[static_cast<int>(i)], pos[static_cast<int>(i)].x, pos[static_cast<int>(i)].y, pos[static_cast<int>(i)].z);
	//h = manager->Play(e, 0, 0, 0);
}
void EffectObj::Plays(float elapsedTime, TYPE i, int num, DirectX::XMFLOAT3 h, float time)
{
	if (straightFlag)
	{
		this->time += elapsedTime;
		if (this->time >= time)
		{
			pos[static_cast<int>(i)].x += h.x * count;
			pos[static_cast<int>(i)].x += h.y * count;
			pos[static_cast<int>(i)].z += h.z * count;

			manager->SetLocation(handle[static_cast<int>(i)], pos[static_cast<int>(i)].x, pos[static_cast<int>(i)].y, pos[static_cast<int>(i)].z);//位置
			manager->SetRotation(handle[static_cast<int>(i)], angle[static_cast<int>(i)].x, angle[static_cast<int>(i)].y, angle[static_cast<int>(i)].z);
			handle[static_cast<int>(i)] = manager->Play(effect[static_cast<int>(i)], 0, 0, 0);
			count++;
			this->time = 0;
		}
		if (count == num)
		{
			straightFlag = false;
			this->time = 0;
			count = 0;
		}
	}

}
void EffectObj::Stop(TYPE i)
{
	//manager->SetPaused(handle[static_cast<int>(i)], true);
	manager->StopEffect(handle[static_cast<int>(i)]);

}