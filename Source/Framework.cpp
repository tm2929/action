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

	// �X���b�v�`�F�[�����쐬���邽�߂̐ݒ�I�v�V����
	DXGI_SWAP_CHAIN_DESC    desc = {};
	{
		desc.BufferCount = 1;					// �o�b�t�@�̐�
		desc.BufferDesc.Width = SCREEN_WIDTH;	// �o�b�t�@�̉���
		desc.BufferDesc.Height = SCREEN_HEIGHT;	// �o�b�t�@�̏c��
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �J���[�t�H�[�}�b�g
		desc.BufferDesc.RefreshRate.Numerator = 60;		// ���t���b�V�����[�g�̕���
		desc.BufferDesc.RefreshRate.Denominator = 1;		// ���t���b�V�����[�g�̕��q
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;		// �o�b�N�o�b�t�@�̐�
		desc.OutputWindow = hwnd;	// DirectX�ŕ`�������\������E�C���h�E
		desc.Windowed = TRUE;		// �E�C���h�E���[�h���A�t���X�N���[���ɂ��邩�B
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	}

	D3D_FEATURE_LEVEL feature_level;
	HRESULT function;
	function = D3D11CreateDeviceAndSwapChain(
		nullptr,	// �ǂ̃r�f�I�A�_�v�^���g�p���邩�H����Ȃ��nullptr�ŁAIDXGIAdapter�̃A�h���X��n��.
		D3D_DRIVER_TYPE_HARDWARE,	// �h���C�o�̃^�C�v��n���B����ȊO�͊�{�I�Ƀ\�t�g�E�F�A�����ŁA�ǂ����Ă��Ƃ�������f�o�O�p�ɗp����ׂ�.
		nullptr,	// ��L��D3D_DRIVER_TYPE_SOFTWARE�ɐݒ肵���ۂɁA���̏������s��DLL�̃n���h����n���B����ȊO���w�肵�Ă���ۂɂ͕K��nullptr��n��.
		create_device_flags,			// ���炩�̃t���O���w�肷��B�ڂ�����D3D11_CREATE_DEVICE�񋓌^�Ō���������.
		feature_levels,	// ���͂�����D3D_FEATURE_LEVEL�񋓌^�̔z���^����Bnullptr�ɂ��邱�Ƃŏ�Lfeature�Ɠ����̓��e�̔z�񂪎g�p�����.
		ARRAYSIZE(feature_levels),			// ��L�����ŁA�����Œ�`�����z���^���Ă����ꍇ�A���̔z��̗v�f���������ɋL�q����.
		D3D11_SDK_VERSION,	// SDK�̃o�[�W�����B�K�����̒l.
		&desc,		// DXGI_SWAP_CHAIN_DESC�\���̂̃A�h���X��ݒ肷��B�����Őݒ肵���\�����ɐݒ肳��Ă���p�����[�^��SwapChain���쐬�����.
		swapChain.GetAddressOf(),	// �쐬�����������ꍇ�ɁA����SwapChain�̃A�h���X���i�[����|�C���^�ϐ��ւ̃A�h���X�B�����Ŏw�肵���|�C���^�ϐ��o�R��SwapChain�𑀍삷��.
		&device,	// ��L�Ƃقړ��l�ŁA������ɂ�Device�̃|�C���^�ϐ��̃A�h���X��ݒ肷��.
		&feature_level,		// ���ۂɍ쐬�ɐ�������D3D_FEATURE_LEVEL���i�[���邽�߂�D3D_FEATURE_LEVEL�񋓌^�ϐ��̃A�h���X��ݒ肷��.
		&deviceContext	// SwapChain��Device�Ɠ��l�ɁA������ɂ�Context�̃|�C���^�ϐ��̃A�h���X��ݒ肷��.
	);
	if (FAILED(function)) {
		assert(0);
		return false;
	}
	// �����_�[�^�[�Q�b�g�r���[�̐���
		// �X���b�v�`�F�[������o�b�N�o�b�t�@�e�N�X�`�����擾����B
		// ���X���b�v�`�F�[���ɓ����Ă���o�b�N�o�b�t�@�e�N�X�`����'�F'���������ރe�N�X�`���B

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

	// �[�x�X�e���V���r���[�̐���
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
	// �r���[�|�[�g�̐ݒ�
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

