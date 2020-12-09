#pragma once
#include <d3d11.h>
#include <wrl.h>
#include<memory>
#include "constant_buffer.h"
#include <Shlwapi.h>

#include "Framebuffer.h"
#include "Sampler.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "Rasterizer.h"
class Bloom
{
public:

	Bloom(ID3D11Device* device, unsigned int width, unsigned int height);
	~Bloom() = default;
	Bloom(Bloom&) = delete;
	Bloom& operator =(Bloom&) = delete;

	void Bloom::Generate(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* hdr_texture);
	void Blit(ID3D11DeviceContext* immediate_context);
private:
	struct ShaderConstants
	{
		float growExtractionThreshold = 1.500f;;
		float blurConvolutionIntensity = 0.500f;
		float dummy;
		float dummy2;
	};
	std::unique_ptr<ConstantBuffer<ShaderConstants>>constantBuffer;

public:
	const std::unique_ptr<ConstantBuffer<ShaderConstants>>& GetConstantBuffer()const { return constantBuffer; }
	void SetGrowExtractionThreshold(float growExtractionThreshold)
	{
		constantBuffer->data.growExtractionThreshold = growExtractionThreshold;
	}
	void SetBlurConvolutionIntensity(float blurConvolutionIntensity)
	{
		constantBuffer->data.blurConvolutionIntensity = blurConvolutionIntensity;
	}
	float GetGrowExtractionThreshold() const { return constantBuffer->data.growExtractionThreshold; }
	//const DirectX::XMFLOAT3& GetColor() { return color; }
	std::unique_ptr<FrameBuffer> glowExtraction;
	std::unique_ptr<FrameBuffer> gaussianBlur[5][2];//‚Ú‚©‚µ

	std::unique_ptr<FullscreenQuad>fullscreenQuad;

	enum { LINEAR_BORDER, POINT, LINEAR, ANISOTROPIC };
	std::unique_ptr<Sampler> samplerStates[4];
	std::unique_ptr<BlendState> blendState;

	Microsoft::WRL::ComPtr<ID3D11PixelShader>	defaultPixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	glowExtractionShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	gaussianBlurHorizontalShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	gaussianBlurVerticalShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	gaussianBlurConvolutionShader;
};