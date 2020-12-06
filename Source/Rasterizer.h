#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXTex.h>

class Rasterizer
{
public:
	Rasterizer(ID3D11Device* device, D3D11_FILTER sampler_filter = D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_MODE sampler_texture_address_mode = D3D11_TEXTURE_ADDRESS_BORDER, DirectX::XMFLOAT4 sampler_boarder_colour = DirectX::XMFLOAT4(0, 0, 0, 0));
	virtual ~Rasterizer() = default;
	Rasterizer(Rasterizer&) = delete;
	Rasterizer& operator=(Rasterizer&) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> embeddedVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> embeddedInputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> embeddedPixelShaders;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> embeddedRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> embeddedDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> embeddedSamplerState;

public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
		DirectX::XMFLOAT4 colour;
	};

	void Blit
	(
		ID3D11DeviceContext* immediate_context,
		ID3D11ShaderResourceView* shaderResourceView,
		float dx, float dy, float dw, float dh,
		float sx, float sy, float sw, float sh,
		float angle/*degree*/,
		float r, float g, float b, float a,
		bool useEmbeddedVertexShader = true,
		bool useEmbeddedPixelShader = true,
		bool useEmbeddedRasterizerState = true,
		bool useEmbeddedDepthStencilState = true,
		bool useEmbeddedSamplerState = true
	) const;
	void Blit(
		ID3D11DeviceContext* immediate_context,
		ID3D11ShaderResourceView* shader_resource_view,
		float dx, float dy, float dw, float dh,
		float angle = 0/*degree*/,
		float r = 1, float g = 1, float b = 1, float a = 1,
		bool use_embedded_vertex_shader = true,
		bool use_embedded_pixel_shader = true,
		bool useEmbeddedRasterizerState = true,
		bool useEmbeddedDepthStencilState = true,
		bool use_embedded_sampler_state = true
	) const;
};

//'fullscreen_quad' dose not have pixel shader and sampler state. you have to make and set pixel shader and sampler state by yourself.
class FullscreenQuad //‰æ–Ê‘S•”‚É‚©‚¯‚Ä‚é
{
public:
	FullscreenQuad(ID3D11Device* device);
	virtual ~FullscreenQuad() = default;
	FullscreenQuad(FullscreenQuad&) = delete;
	FullscreenQuad& operator=(FullscreenQuad&) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> embeddedVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> embeddedPixelShaders;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> embeddedRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> embeddedDepthStencilState;

public:
	void Blit(ID3D11DeviceContext* immediate_contextbool, bool useEmbeddedRasterizerState = true, bool useEmbeddedDepthStencilState = true, bool use_embedded_pixel_shader = false);
};