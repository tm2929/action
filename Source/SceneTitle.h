#pragma once
#include "Scene.h"
#include<memory>
#include <thread>
#include <mutex>
#include "Obj3D.h"
#include "ModelRenderer.h"
#include "Sprite.h"
#include "BlendState.h"
#include "Sound.h"
#include "Sampler.h"
#include "RasterizerState.h"
#include "ShaderEX.h"
#include "Framebuffer.h"
#include "Bloom.h"
#include "RenderEffects.h"
class  SceneTitle : public Scene
{
private:
	std::unique_ptr<FrameBuffer>frameBuffer[2]; //0 本体　1シェーダー書き込み
	std::unique_ptr<Bloom> bloomEffect;
	std::unique_ptr<RenderEffects>renderEffects;
	std::unique_ptr<FullscreenQuad>fullscreenQuad;
	DirectX::XMFLOAT4 dissolveColor = { 1,0,0,0 };
	float dissolveThreshold = 0;
	float dissolveEmissiveWidth = 0.15f;
	float dissolveThresholdPlus = 0.5f;
	float sceneChangeTimer = 1.5f;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	std::unique_ptr<RasterizerState>rasterizer;
	std::shared_ptr<Sampler>samplerWrap;
	//std::unique_ptr<SoundManager>soundManager;
	std::unique_ptr<ModelRenderer>modelRenderer;
	std::unique_ptr<Obj3D> obj;
	std::unique_ptr<BlendState> blend; 
	std::unique_ptr<Sprite>titleTex;
	DirectX::XMFLOAT3 color = { 1,1,1 };
	std::unique_ptr<ShaderEx>dissolveShader;
	bool colorChangeFlag = false;
	std::unique_ptr<Sprite>keyTex;
	std::unique_ptr<Sprite>buttonTex;

	float colorTime[4] = { 1,1.5f,2.5f,3.0f };
	float colorTimerSpeed = 2.0f;
	float colorTimer;
	float texColorW = 1.0f;
public:
	std::unique_ptr<std::thread> loading_thread;
	std::mutex loading_mutex;

	bool IsNowLoading();
	void EndLoading();
	
	SceneTitle(ID3D11Device* device, HWND hwnd);
	int Update(float elapsed_time);
	void Render(float elapsed_time, ID3D11DeviceContext* devicecontext);
	~SceneTitle();
};

