#pragma once

#include <d3d11.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include <assert.h>

class SamplerState
{
public:
	ComPtr<ID3D11SamplerState> samplerState;

	SamplerState(ID3D11Device* device, int num)
	{
		D3D11_SAMPLER_DESC desc;
		{
			ZeroMemory(&desc, sizeof(desc));
			switch (num)
			{
			case 0://sprite用
				//拡大縮小時の色の取得方法
				desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;//縮小、拡大にポイント サンプリングを使用し、ミップレベルのサンプリングに線形補間を使用します。
				//テクスチャのU座標が0～1の範囲外の場合の色の取得方法
				//https://www.3dgep.com/texturing-lighting-directx-11/
				desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;//0.0 または 1.0 のテクスチャー カラーが設定されます。
				desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
				desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
				desc.MipLODBias = 0;
				desc.MaxAnisotropy = 16;
				desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
				desc.MinLOD = 0;
				desc.MaxLOD = D3D11_FLOAT32_MAX;
				break;
			case 1://bath用
				desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
				desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
				desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
				desc.MipLODBias = 0;
				desc.MaxAnisotropy = 16;
				desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
				desc.MinLOD = 0;
				desc.MaxLOD = D3D11_FLOAT32_MAX;
				break;
			case 2: // Shadow Map
				desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
				desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
				desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
				desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
				//ボーダーカラー
				desc.BorderColor[0] = 1.0f;
				desc.BorderColor[1] = 1.0f;
				desc.BorderColor[2] = 1.0f;
				desc.BorderColor[3] = 1.0f;
				break;
			}

			HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
			if (FAILED(hr))
			{
				assert(!"CreateSamplerState");
			}
		}
	}
	void Activate(ID3D11DeviceContext* context, unsigned int slot)
	{
		context->PSSetSamplers(slot, 1, samplerState.GetAddressOf());
	}
};
