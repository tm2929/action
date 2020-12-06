#pragma once
#include"scene.h"
#include <thread>
#include <mutex>


class SceneManager
{
private:
	static SceneManager* scenemanager;
	std::unique_ptr<Scene> currentScene;
	SceneManager(ID3D11Device* device, HWND hwnd);
public:
	void ChangeScene(int next_scene, ID3D11Device* device, HWND hwnd);

	enum SCENE
	{
		TITLE = 1,
		GAME,
		OVER,
		CLEAR,
	};
	static void Create(ID3D11Device* device, HWND hwnd)
	{
		if (scenemanager != nullptr)
			return;

		scenemanager = new SceneManager(device,  hwnd);

	}
	static SceneManager& GetInstance()
	{
		return *scenemanager;
	}
	static void Destroy()
	{
		if (scenemanager != nullptr)
		{
			delete scenemanager;
			scenemanager = nullptr;
		}
	}
	void Updata(float elapsed_time, ID3D11Device* device, HWND hwnd);
	void Render(float elapsed_time, ID3D11DeviceContext* deviceContext);
	~SceneManager() {
	}
};
#define pSceneManager (SceneManager::GetInstance())