#pragma once

#include "obj3d_manager.h"
#include "actor.h"
#include "judge.h"

class Sky : public Actor
{
public:
	void Move(Obj3D* obj, float elapsedTime);
};

class SkyStage : public EraseAlg
{
	void Erase(Obj3D* obj, float elapsedTime) { obj->Release(); }
};

class SkyManager : public Obj3DManager
{
	static SkyManager* instance;

public:
	void Init(ID3D11Device* device);
	void Update(float elapsedTime);

	static void Create()
	{
		if (instance != nullptr) return;
		instance = new SkyManager();
	}
	static SkyManager& GetInstance()
	{
		return *instance;
	}
	static void Destory()
	{
		if (instance != nullptr)
		{
			delete instance;
			instance = nullptr;
		}
	}
};

