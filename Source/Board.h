#pragma once
#include<d3d11.h>
#include <DirectXMath.h>

class Board
{

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
		DirectX::XMFLOAT4 color;
	};
	struct cbuffer
	{
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4 materialColor;
	};

	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11Buffer* buffer = nullptr;
	ID3D11RasterizerState* rasterrizer = nullptr;
	ID3D11ShaderResourceView* srview = nullptr;
	D3D11_TEXTURE2D_DESC texture2d;
	ID3D11SamplerState* samplerState = nullptr;
	ID3D11DepthStencilState* depthStencil = nullptr;
	ID3D11InputLayout* input = nullptr;
	ID3D11Buffer* constantBuffer;

public:
	Board(ID3D11Device* device, const wchar_t* file_name);
	//~Board();
	void render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT3& position, const float& scale, const DirectX::XMFLOAT3& angle, const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& color);
};
