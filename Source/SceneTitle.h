#pragma once
#include "Scene.h"
#include<memory>
#include <thread>
#include <mutex>
#include "Obj3D.h"
#include "ModelRenderer.h"
#include "Sprite.h"
#include "BlendState.h"
class  SceneTitle : public Scene
{
private:
	std::unique_ptr<ModelRenderer>modelRenderer;
	std::unique_ptr<Obj3D> obj;
	std::unique_ptr<BlendState> blend; 
	std::unique_ptr<Sprite>titleTex;
	DirectX::XMFLOAT3 color = { 1,1,1 };
	bool colorChangeFlag = false;
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
	
	SceneTitle(ID3D11Device* device, HWND hwnd);
	int Update(float elapsed_time);
	void Render(float elapsed_time, ID3D11DeviceContext* devicecontext);
	~SceneTitle();
};

