#pragma once
#include <memory>
#include <d3d11.h>
#include "model.h"
#include "vector.h"
#include "constant_buffer.h"
class ModelRenderer
{
public:
	ModelRenderer(ID3D11Device* device, bool depthFlag = false);
	~ModelRenderer();


	void Begin(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& light_direction);
	void Draw(ID3D11DeviceContext* context, Model* model, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1, 1, 1, 1), const DirectX::XMFLOAT4& lightObj = DirectX::XMFLOAT4(0, 0, 0, 0));
	void End(ID3D11DeviceContext* context);
	void ShadowBegin(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view_projection);
	void ShadowDraw(ID3D11DeviceContext* context, Model& model, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1, 1, 1, 1));
	void ShadowEnd(ID3D11DeviceContext* context);
private:
	static const int MaxBones = 128;

	struct FogShaderConstants
	{
		float fogNear = 10;
		float fogFar = 500;
		float dummy0;
		float dummy1;
		DirectX::XMFLOAT4 fogColor = { 0.5f,0.5f,0.5f,1.0f };
	};
	std::unique_ptr<ConstantBuffer<FogShaderConstants>>fogBuffer;
public:
	void SetFogNear(float fogNear)
	{
		fogBuffer->data.fogNear = fogNear;
	}
	void SetFogFar(float fogFar)
	{
		fogBuffer->data.fogFar = fogFar;
	}
	void SetFogColor(DirectX::XMFLOAT4 fogColor)
	{
		fogBuffer->data.fogColor = fogColor;
	}
	struct CbScene
	{
		DirectX::XMFLOAT4X4	view_projection;
		DirectX::XMFLOAT4	light_direction;

	};
	struct CbObj
	{
		DirectX::XMFLOAT4	light_direction;
	};
	struct CbMesh
	{
		DirectX::XMFLOAT4X4	bone_transforms[MaxBones];
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4 	material_color;
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer>			cbScene;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cbObj;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cbMesh;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cbSubset;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		shadowShader;


	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		shadow_input;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState02;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	//Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	dummySrv;
};
