
#include "light.h"
#include <d3d11.h>


VECTOR4 Light::LightDir(1, 1, 1, 0);
VECTOR4 Light::DirLightColor(1, 1, 1, 1);
VECTOR4 Light::Ambient(1, 1, 1, 1);
POINTLIGHT Light::PointLight[POINTMAX];
SPOTLIGHT Light::SpotLight[SPOTMAX];
std::unique_ptr<ConstantBuffer<Light::CBLight>> Light::cbLight;

void Light::Init(ID3D11Device* device)
{
	ZeroMemory(PointLight, sizeof(POINTLIGHT) * POINTMAX);
	ZeroMemory(SpotLight, sizeof(SPOTLIGHT) * SPOTMAX);

	//cbLight = std::make_unique<ConstantBuffer<CBLight>>(device);
}

void Light::SetDirLight(VECTOR3 dir, VECTOR3 color)
{
	float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

	if (d > 0) { dir.x /= d, dir.y /= d, dir.z /= d; }
	LightDir = VECTOR4(dir.x, dir.y, dir.z, 0);
	DirLightColor = VECTOR4(color.x, color.y, color.z, 1);
}

void Light::SetAmbient(VECTOR3 amb)
{
	Ambient = { amb.x,amb.y,amb.z,0 };
}

void Light::SetPointLight(int index, VECTOR3 pos, VECTOR4 color, float range)
{
	if (index < 0) return;
	if (index >= POINTMAX)return;
	PointLight[index].index = (float)index;
	PointLight[index].range = range;
	PointLight[index].type = 1.0f;
	PointLight[index].dumy = 0.0f;
	PointLight[index].pos = VECTOR4(pos.x, pos.y, pos.z, 0);
	PointLight[index].color = VECTOR4(color.x, color.y, color.z, color.w);
}

void Light::SetSpotLight(int index, VECTOR3 pos, VECTOR4 color, VECTOR3 dir,
	float range, float near_area, float far_area)
{
	if (index < 0) return;
	if (index >= SPOTMAX)return;
	float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
	if (d > 0) {
		dir.x /= d; dir.y /= d; dir.z /= d;
	}

	SpotLight[index].index = (float)index;
	SpotLight[index].range = range;
	SpotLight[index].type = 1.0f;
	SpotLight[index].near_area = near_area;
	SpotLight[index].far_area = far_area;
	SpotLight[index].dumy0 = 0.0f;
	SpotLight[index].dumy1 = 0.0f;
	SpotLight[index].dumy2 = 0.0f;

	SpotLight[index].pos = VECTOR4(pos.x, pos.y, pos.z, 0);
	SpotLight[index].color = VECTOR4(color.x, color.y, color.z, color.w);
	DirectX::XMVECTOR nDir = DirectX::XMLoadFloat4(&VECTOR4(dir.x, dir.y, dir.z, 0));
	DirectX::XMStoreFloat4(&SpotLight[index].dir, nDir);
}

void Light::SetBuffer()
{
	cbLight->data.lightColor = DirLightColor;
	cbLight->data.lightDir = LightDir;
	cbLight->data.ambientColor = Ambient;
	//cbLight->data.eyePos = ;
}

void Light::Activate(ID3D11DeviceContext* context)
{
	cbLight->Activate(context, 3);
}

void Light::DeActivate(ID3D11DeviceContext* context)
{
	cbLight->DeActivate(context);
}
