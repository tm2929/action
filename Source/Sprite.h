#pragma once
#pragma once

#include<d3d11.h>


#include <DirectXMath.h>
#include"ResourceManager.h"


class Sprite {
public:
	ID3D11VertexShader* vertexshader;
	ID3D11PixelShader* pixe;
	ID3D11InputLayout* input;
	ID3D11Buffer* buffer;
	ID3D11RasterizerState* rasterrizer;
	ID3D11ShaderResourceView* srview;
	D3D11_TEXTURE2D_DESC texture2d;
	ID3D11SamplerState* samplerstate;
	ID3D11DepthStencilState* depthstencil;
	ID3D11BlendState* blend;

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
	//~Sprite();


	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
};