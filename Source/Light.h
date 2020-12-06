#pragma once

#include "vector.h"
#include "constant_buffer.h"
#include <memory>

struct SPOTLIGHT {
	float index = 0.f;
	float range = 0.f;//Œõ‚Ì“Í‚­”ÍˆÍ
	float type = 0.f; //—LŒø‚©–³Œø‚©
	float near_area = 0.f;
	float far_area = 0.f;
	float dumy0 = 0.f;
	float dumy1 = 0.f;
	float dumy2 = 0.f;
	VECTOR4 pos = VECTOR4(0, 0, 0, 0);
	VECTOR4 color = VECTOR4(1.f, 1.f, 1.f, 1.f);
	VECTOR4 dir = VECTOR4(0, 0, 0, 0);
};

struct POINTLIGHT {
	float index = 0.f;
	float range = 0.f;//Œõ‚Ì“Í‚­”ÍˆÍ
	float type = 0.f; //—LŒø‚©–³Œø‚©
	float dumy = 0.f;
	VECTOR4 pos = VECTOR4(0, 0, 0, 0);
	VECTOR4 color = VECTOR4(1.f, 1.f, 1.f, 1.f);
};

class Light
{
public:
	static const int POINTMAX = 32;
	static const int SPOTMAX = 32;

	static VECTOR4 LightDir;
	static VECTOR4 DirLightColor;
	static VECTOR4 Ambient;
	static POINTLIGHT PointLight[POINTMAX];
	static SPOTLIGHT SpotLight[SPOTMAX];

	struct CBLight
	{
		VECTOR4 lightColor;
		VECTOR4 lightDir;
		VECTOR4 ambientColor;
		VECTOR4 eyePos;


	};
	static std::unique_ptr<ConstantBuffer<CBLight>> cbLight;

	static void Init(ID3D11Device* device);
	static void SetDirLight(VECTOR3 dir, VECTOR3 color);

	static void SetAmbient(VECTOR3 amb);

	static void SetPointLight(int index, VECTOR3 pos, VECTOR4 color, float range);

	static void SetSpotLight(int index, VECTOR3 pos, VECTOR4 color, VECTOR3 dir, float range, float near, float far);

	static void SetBuffer();
	static void Activate(ID3D11DeviceContext* context);
	static void DeActivate(ID3D11DeviceContext* context);
};
