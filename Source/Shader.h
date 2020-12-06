#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
using namespace DirectX;

class Shader
{
public:
	Shader() {}
	Shader(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& immediateContext);
	bool Create(ID3D11Device* device, ID3D11DeviceContext* device_context, char * vsName,  char* psName, UINT numElements, D3D11_INPUT_ELEMENT_DESC* input_elemen);
	virtual ~Shader() {}

	bool CreateSprite(ID3D11Device* device);
	bool CreateSpriteBatch(ID3D11Device* device);
	bool CreateBoard(ID3D11Device* device);
	bool CreateGeometric(ID3D11Device* device);
	bool CreateModel(ID3D11Device* device);
	bool CreateMesh(ID3D11Device* device);

	bool CreateObj(ID3D11Device* device);
	bool CreateTexGeometric(ID3D11Device* device);

	void Activate(ID3D11DeviceContext* context);
	void DeActivate(ID3D11DeviceContext* context);
protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		VS = nullptr; // 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		PS = nullptr; // ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	GS = nullptr; // ジオメトリシェーダ
	Microsoft::WRL::ComPtr<ID3D11HullShader>		HS = nullptr; //　ハルシェーダ
	Microsoft::WRL::ComPtr<ID3D11DomainShader>		DS = nullptr;//　ドメインネームシェーダ


	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>		VertexLayout;
};


//----------------------------------------------
//
//	頂点データ構造体定義
//
//----------------------------------------------

struct VERTEX
{
	XMFLOAT3 Pos;	//位置
	XMFLOAT3 Normal;//法線
	XMFLOAT2 Tex;	//UV座標
	XMFLOAT4 Color;	//頂点色
};