#pragma once
#include "Scene.h"
#include<memory>
#include <thread>
#include <mutex>
#include "Obj3D.h"
#include "Sprite.h"
#include "BlendState.h"
class  SceneOver : public Scene
{
private:
	std::unique_ptr<Sprite> over;
	std::unique_ptr<Sprite> retry;
	std::unique_ptr<Sprite> title;
	std::unique_ptr<Sprite> choice;
	std::unique_ptr<Sprite> nowLoading;
	std::unique_ptr<BlendState> blend;

	DirectX::XMFLOAT2 retryPos;
	DirectX::XMFLOAT2 titlePos;

	DirectX::XMFLOAT2 choicePos;

	bool retryFlag;
	float colorTimer;
	float texColorW;
public:
	std::unique_ptr<std::thread> loading_thread;
	std::mutex loading_mutex;

	bool IsNowLoading()
	{
		if (loading_thread && loading_mutex.try_lock())
		{
			loading_mutex.unlock();
			return false;
		}
		return true;
	}
	void EndLoading()
	{
		if (loading_thread && loading_thread->joinable())
		{
			loading_thread->join();
		}
	}

	SceneOver(ID3D11Device* device, HWND hwnd);
	int Update(float elapsed_time);
	void Render(float elapsed_time, ID3D11DeviceContext* devicecontext);
	~SceneOver();
};

