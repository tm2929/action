#include "framework.h"
#include<assert.h>
#include "gamepad.h"
#include "Camera.h"
#include "KeyInput.h"

#ifdef USE_IMGUI
#pragma comment( lib, "./Effekseer_incl/lib/debug/Effekseerd.lib" )
#pragma comment( lib, "./Effekseer_incl/lib/debug/EffekseerRendererDX11d.lib" )
//#pragma comment( lib, "./Effekseer_incl/lib/debug/EffekseerSoundXAudio2d.lib" )

#else

#pragma comment( lib, "./Effekseer_incl/lib/release/Effekseer.lib" )
#pragma comment( lib, "./Effekseer_incl/lib/release/EffekseerRendererDX11.lib" )
//#pragma comment( lib, "./Effekseer_incl/lib/release/EffekseerSoundXAudio2.lib" )
#endif // DEBUG

bool framework::initialize(HWND hwnd)
{
	UINT create_device_flags = 0;

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	// スワップチェーンを作成するための設定オプション
	DXGI_SWAP_CHAIN_DESC    desc = {};
	{
		desc.BufferCount = 1;					// バッファの数
		desc.BufferDesc.Width = SCREEN_WIDTH;	// バッファの横幅
		desc.BufferDesc.Height = SCREEN_HEIGHT;	// バッファの縦幅
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// カラーフォーマット
		desc.BufferDesc.RefreshRate.Numerator = 60;		// リフレッシュレートの分母
		desc.BufferDesc.RefreshRate.Denominator = 1;		// リフレッシュレートの分子
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;		// バックバッファの数
		desc.OutputWindow = hwnd;	// DirectXで描いた画を表示するウインドウ
		desc.Windowed = TRUE;		// ウインドウモードか、フルスクリーンにするか。
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	}

	D3D_FEATURE_LEVEL feature_level;
	HRESULT function;
	function = D3D11CreateDeviceAndSwapChain(
		nullptr,	// どのビデオアダプタを使用するか？既定ならばnullptrで、IDXGIAdapterのアドレスを渡す.
		D3D_DRIVER_TYPE_HARDWARE,	// ドライバのタイプを渡す。これ以外は基本的にソフトウェア実装で、どうしてもという時やデバグ用に用いるべし.
		nullptr,	// 上記をD3D_DRIVER_TYPE_SOFTWAREに設定した際に、その処理を行うDLLのハンドルを渡す。それ以外を指定している際には必ずnullptrを渡す.
		create_device_flags,			// 何らかのフラグを指定する。詳しくはD3D11_CREATE_DEVICE列挙型で検索検索ぅ.
		feature_levels,	// 実はここでD3D_FEATURE_LEVEL列挙型の配列を与える。nullptrにすることで上記featureと同等の内容の配列が使用される.
		ARRAYSIZE(feature_levels),			// 上記引数で、自分で定義した配列を与えていた場合、その配列の要素数をここに記述する.
		D3D11_SDK_VERSION,	// SDKのバージョン。必ずこの値.
		&desc,		// DXGI_SWAP_CHAIN_DESC構造体のアドレスを設定する。ここで設定した構造愛に設定されているパラメータでSwapChainが作成される.
		swapChain.GetAddressOf(),	// 作成が成功した場合に、そのSwapChainのアドレスを格納するポインタ変数へのアドレス。ここで指定したポインタ変数経由でSwapChainを操作する.
		&device,	// 上記とほぼ同様で、こちらにはDeviceのポインタ変数のアドレスを設定する.
		&feature_level,		// 実際に作成に成功したD3D_FEATURE_LEVELを格納するためのD3D_FEATURE_LEVEL列挙型変数のアドレスを設定する.
		&deviceContext	// SwapChainやDeviceと同様に、こちらにはContextのポインタ変数のアドレスを設定する.
	);
	if (FAILED(function)) {
		assert(0);
		return false;
	}
	// レンダーターゲットビューの生成
		// スワップチェーンからバックバッファテクスチャを取得する。
		// ※スワップチェーンに内包されているバックバッファテクスチャは'色'を書き込むテクスチャ。

	ID3D11Texture2D* dtexture2d;
	function = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&dtexture2d);
	if (FAILED(function)) {
		assert(0);
		return false;
	}
	function = device->CreateRenderTargetView(
		dtexture2d,//
		nullptr,
		renderTargetView.GetAddressOf()
	);
	if (FAILED(function)) {
		assert(0);
		return false;
	}
	dtexture2d->Release();

	// 深度ステンシルビューの生成
	{
		D3D11_TEXTURE2D_DESC txDesc;

		txDesc.Width = SCREEN_WIDTH;
		txDesc.Height = SCREEN_HEIGHT;
		txDesc.MipLevels = 1;
		txDesc.ArraySize = 1;
		txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		txDesc.SampleDesc.Count = 1;
		txDesc.SampleDesc.Quality = 0;
		txDesc.Usage = D3D11_USAGE_DEFAULT;
		txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		txDesc.CPUAccessFlags = 0;
		txDesc.MiscFlags = 0;

		function = device->CreateTexture2D(&txDesc, nullptr, texture2d.GetAddressOf());
		if (FAILED(function)) {
			assert(0);
			return false;
		}
#ifdef USE_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(device, deviceContext);

		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
#endif

		function = device->CreateDepthStencilView(texture2d.Get(), nullptr, depthStencilView.GetAddressOf());
	}
	// ビューポートの設定
	D3D11_VIEWPORT dv;
	dv.TopLeftX = 0;
	dv.TopLeftY = 0;
	dv.Width = static_cast<float>(SCREEN_WIDTH);
	dv.Height = static_cast<float>(SCREEN_HEIGHT);
	dv.MinDepth = 0.0f;
	dv.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &dv);

	UINT m;
	function = device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m);
	if (FAILED(function)) {
		assert(0);
		return false;
	}


	pSceneManager.Create(device,hwnd);
	Camera::GetInstance().SetViewPort(deviceContext);
	return true;
}
void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
	input::GamepadUpdate();
	KeyInput::Update();
#ifdef USE_IMGUI

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

#endif

	pSceneManager.Updata(elapsed_time, device, hwnd);
	//ImGui::SetNextWindowSize(ImVec2(320, 100), ImGuiSetCond_Once);

}
void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{

	float ClearColor[4] = { 0.0f, 0.f, 0.0f, 1.0f }; //red,green,blue,alpha
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), ClearColor);

	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	pSceneManager.Render(elapsed_time, deviceContext);
#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
	swapChain->Present(1, 0);

}

