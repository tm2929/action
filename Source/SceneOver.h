#pragma once
#include "Scene.h"
#include<memory>
#include <thread>
#include <mutex>
#include "Obj3D.h"
#include "Sprite.h"
#include "BlendState.h"

#include "ShaderEX.h"
#include "Framebuffer.h"
#include "Bloom.h"
#include "RenderEffects.h"
class  SceneOver : public Scene
{
private:
	std::unique_ptr<FrameBuffer>frameBuffer[2]; //0 本体　1シェーダー書き込み
	std::unique_ptr<Bloom> bloomEffect;
	std::unique_ptr<RenderEffects>renderEffects;
	std::unique_ptr<FullscreenQuad>fullscreenQuad;
	DirectX::XMFLOAT4 dissolveColor = { 1,0,0,0 };
	float dissolveThreshold = 0;
	float dissolveEmissiveWidth = 0.15f;
	float dissolveThresholdPlus = 0.4f;
	float sceneChangeTimer = 1.5f;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	std::unique_ptr<ShaderEx>dissolveShader;
	bool dissolveFlag = false;

	std::unique_ptr<Sprite> overTex;
	std::unique_ptr<Sprite> retryTex;
	std::unique_ptr<Sprite> titleTex;
	std::unique_ptr<Sprite> choiceTex;
	std::unique_ptr<Sprite> nowLoadingTex;
	std::unique_ptr<BlendState> blend;



	DirectX::XMFLOAT2 choicePos;

	float colorTime[4] = { 1,1.5f,2.5f,3.0f };
	float colorTimerSpeed = 2.0f;
	bool retryFlag;
	float colorTimer;
	float texColorW;
	const float retryPos = 1146;
	const float titlePos = 222;
public:
	std::unique_ptr<std::thread> loading_thread;
	std::mutex loading_mutex;

	bool IsNowLoading();
	void EndLoading();

	SceneOver(ID3D11Device* device, HWND hwnd);
	int Update(float elapsed_time);
	void Render(float elapsed_time, ID3D11DeviceContext* devicecontext);
	~SceneOver();
};

