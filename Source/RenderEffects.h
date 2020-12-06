#pragma once
#pragma once
#include<d3d11.h>
#include<wrl.h>
#include"vector.h"

class RenderEffects
{
public:
	RenderEffects(ID3D11Device* device);
	void ShadowRender(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorMapSRV, ID3D11ShaderResourceView* depthMapSRV, ID3D11ShaderResourceView* shadowMapSRV
		, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4X4& lightView, const DirectX::XMFLOAT4X4& lightProjection);
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSamplerState[2];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	struct CbScene
	{
		FLOAT4X4 inverseViewProjection;
		FLOAT4X4 lightViewProjection;
		VECTOR3 shadowColor = VECTOR3(0.65f, 0.65f, 0.65f);
		float shadowbisa = 0.0008f;
	};
};