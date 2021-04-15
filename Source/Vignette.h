#pragma once
#include <d3d11.h>
#include <wrl.h>
#include<memory>
#include "constant_buffer.h"
#include <Shlwapi.h>

#include "Framebuffer.h"
#include "Sampler.h"
#include "Rasterizer.h"
#include "vector.h"
class Vignette
{
public:
	struct ShaderConstants
	{
		float vignetteArea = 1.5f;
		float vignetteSmooth = 1.0f;
		float dummy;//float4ŠÔŠu
		float dummy2;
		DirectX::XMFLOAT4  vignetteColor = { 1,0,0,1 };
	};
	Vignette(ID3D11Device* device, unsigned int width, unsigned int height);
	~Vignette() = default;
	Vignette(Vignette&) = delete;
	Vignette& operator =(Vignette&) = delete;

	void Vignette::Render(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* hdr_texture);;

	std::unique_ptr<FrameBuffer> framebuffer;
	std::unique_ptr<FullscreenQuad>fullscreenQuad;
private:
	std::unique_ptr<ConstantBuffer<ShaderConstants>>constantBuffer;
	std::unique_ptr<Sampler> samplerState;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

public:
	void SetVignetteArea(float vignetteArea)
	{
		constantBuffer->data.vignetteArea = vignetteArea;
	}
	void SetVignetteSmooth(float vignetteSmooth)
	{
		constantBuffer->data.vignetteSmooth = vignetteSmooth;
	}
	void SetVignetteColor(DirectX::XMFLOAT4 vignetteColor)
	{
		constantBuffer->data.vignetteColor = vignetteColor;
	}
	const float& GetVignetteArea() { return constantBuffer->data.vignetteArea; }

};