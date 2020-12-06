#pragma once
#include "SceneManager.h"
#include "Fadeout.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "SceneClear.h"
#include "SceneOver.h"
#include<Windows.h>
SceneManager* SceneManager::scenemanager = nullptr;
SceneManager::SceneManager(ID3D11Device* device, HWND hwnd)
{
	pFadeOut.Load(device, L"Data/images/siro.png");
	ChangeScene(TITLE, device, hwnd);
}
void SceneManager::Updata(float elapsed_time, ID3D11Device* device, HWND hwnd)
{

	int scene_chang = currentScene->Update(elapsed_time);
	if (scene_chang > 0)
	{
		ChangeScene(scene_chang, device, hwnd);
	}
}

void SceneManager::ChangeScene(int nextScene, ID3D11Device* device, HWND hwnd)
{
	if (currentScene)
	{
		currentScene = nullptr;
	}
	switch (nextScene)
	{
	case SCENE::TITLE:
		currentScene = std::make_unique<SceneTitle>(device, hwnd);
		break;
	case SCENE::GAME:
		currentScene = std::make_unique<SceneGame>(device, hwnd);
		break;
	case SCENE::OVER:
		currentScene = std::make_unique<SceneOver>(device, hwnd);
		break;
	case SCENE::CLEAR:
		currentScene = std::make_unique <SceneClear>(device, hwnd);
		break;

	}
}

void SceneManager::Render(float elapsed_time, ID3D11DeviceContext* devicecontext)
{
	currentScene->Render(elapsed_time, devicecontext);
}
