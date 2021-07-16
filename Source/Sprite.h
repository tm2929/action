#pragma once

#include<d3d11.h>
#include <DirectXMath.h>
#include"ResourceManager.h"
#include "ShaderEX.h"
#include <memory>
#include "constant_buffer.h"

class Sprite {
public:


	void Render(ID3D11DeviceContext*
		//// UNIT.03   
		, float, float, // dx, dy : Coordinate of sprite's left-top corner in screen space    
		float, float,  // dw, dh : Size of sprite in screen space   
		// UNIT.04 
		float, float, // sx, sy : Coordinate of sprite's left-top corner in texture space    
		float, float,  // sw, sh : Size of sprite in texture space   
		float, // angle : Raotation angle (Rotation centre is sprite's centre), Unit is degree   
		float, float, float, float // r, g, b, a : Color of sprite's each vertices  
	);

	//Dissolve用レンダー
	void DissolveRender(ID3D11DeviceContext*
		, Shader*
		, ID3D11ShaderResourceView*
		//// UNIT.03   
		, float, float, // dx, dy : Coordinate of sprite's left-top corner in screen space    
		float, float,  // dw, dh : Size of sprite in screen space   
		// UNIT.04 
		float, float, // sx, sy : Coordinate of sprite's left-top corner in texture space    
		float, float,  // sw, sh : Size of sprite in texture space   
		float, // angle : Raotation angle (Rotation centre is sprite's centre), Unit is degree   
		float, float, float, float // r, g, b, a : Color of sprite's each vertices  
	);

	//外部からシェーダーを読み込む
	void Render(ID3D11DeviceContext*
		, Shader*
		//// UNIT.03   
		, float, float, // dx, dy : Coordinate of sprite's left-top corner in screen space    
		float, float,  // dw, dh : Size of sprite in screen space   
		// UNIT.04 
		float, float, // sx, sy : Coordinate of sprite's left-top corner in texture space    
		float, float,  // sw, sh : Size of sprite in texture space   
		float, // angle : Raotation angle (Rotation centre is sprite's centre), Unit is degree   
		float, float, float, float // r, g, b, a : Color of sprite's each vertices  
	);

	void Render(ID3D11DeviceContext*,
		ID3D11ShaderResourceView*
		, float, float, // dx, dy : Coordinate of sprite's left-top corner in screen space    
		float, float,  // dw, dh : Size of sprite in screen space   
		// UNIT.04 
		float, float, // sx, sy : Coordinate of sprite's left-top corner in texture space    
		float, float,  // sw, sh : Size of sprite in texture space   
		float, // angle : Raotation angle (Rotation centre is sprite's centre), Unit is degree   
		float, float, float, float // r, g, b, a : Color of sprite's each vertices  
	);
	Sprite(ID3D11Device*, const wchar_t* /*Texture file name*/);

private:
	struct ShaderConstants
	{
		float  dissolveThreshold = 0;
		float emissiveWith = 0;
		float dummy0;
		float dummy1;
		DirectX::XMFLOAT4 dissolveColor = { 1,1,1,1 };
	};
	std::unique_ptr<ConstantBuffer<ShaderConstants>>constantBuffer;
public:
	const std::unique_ptr<ConstantBuffer<ShaderConstants>>& GetConstantBuffer()const { return constantBuffer; }
	void SetDissolveThreshold(float threshold)
	{
		constantBuffer->data.dissolveThreshold = threshold;
	}
	void SetEmissiveWidth(float emissiveWith)
	{
		constantBuffer->data.emissiveWith = emissiveWith;
	}
	void SetDissolveColor(DirectX::XMFLOAT4 dissolveColor)
	{
		constantBuffer->data.dissolveColor = dissolveColor;
	}
private:
	Microsoft::WRL::ComPtr <ID3D11VertexShader > vertexShader;
	Microsoft::WRL::ComPtr < ID3D11PixelShader >pixelShader;
	Microsoft::WRL::ComPtr < ID3D11InputLayout >inputLayout;
	Microsoft::WRL::ComPtr <ID3D11SamplerState>samplerstate;
	ID3D11Buffer* buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srview;
	D3D11_TEXTURE2D_DESC texture2d;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
};