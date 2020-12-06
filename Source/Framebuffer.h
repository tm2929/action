#pragma once
#include<d3d11.h>
#include <wrl.h>
#include <assert.h>
#include <memory>

class FrameBuffer
{
public:
	FrameBuffer(ID3D11Device* device, int width, int height, bool msaaFlag = false, int subsamples = 1, DXGI_FORMAT renderTargetTexture2dFormat = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT depthStencilTexture2dFormat = DXGI_FORMAT_R24G8_TYPELESS,
		bool renderTargetShaderResourceViewFlag = true, bool depthStencilShaderResourceViewFlag = true, bool mipsFlag = false);
	//èâä˙âª
	void Clear(ID3D11DeviceContext* context, float r = 0, float g = 0, float b = 0, float a = 1, unsigned int clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, float depth = 1, unsigned char stencil = 0);
	//äJén
	void Activate(ID3D11DeviceContext* context);
	//èIóπ
	void Deactivate(ID3D11DeviceContext* context);

	//getter
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetRenderTargetShaderResourceView() { return mRenderTargetShaderResourceView; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetDepthStencilShaderResourceView() { return mDepthStencilShaderResourceView; }
private:
	//render target view data
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>mRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mRenderTargetShaderResourceView;
	//depth stencil view data
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>mDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mDepthStencilShaderResourceView;
	//default view data
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>mDefaultRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>mDefaultDepthStencilView;
	unsigned int viewportsNumber;
	D3D11_VIEWPORT defaultViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	D3D11_VIEWPORT viewport;

};