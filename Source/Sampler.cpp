
#include "Sampler.h"
#include <d3d11.h>
#include <cassert>

Sampler::Sampler(ID3D11Device* device, D3D11_SAMPLER_DESC sd)
{

	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//FLOAT color[4] = { r,g,b,a };
	//sd.BorderColor = color;

	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = device->CreateSamplerState(&sd, stateObject.GetAddressOf());
	assert(SUCCEEDED(hr));

}

Sampler::Sampler(
	ID3D11Device* device,
	D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE Address_mode,
	FLOAT color[4])
{
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = filter;
	sd.AddressU = Address_mode;
	sd.AddressV = Address_mode;
	sd.AddressW = Address_mode;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	if (color) {
		memcpy(sd.BorderColor, color, sizeof(color));
	}

	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = device->CreateSamplerState(&sd, stateObject.GetAddressOf());
	assert(SUCCEEDED(hr));
}
//void Sampler::Set(ID3D11DeviceContext* deviceContext,UINT Slot, BOOL flg)
//{
//
//	if (samplerState) {
//		deviceContext->PSSetSamplers(Slot, 1, samplerState.GetAddressOf());
//		deviceContext->DSSetSamplers(Slot, 1, samplerState.GetAddressOf());
//	}
//
//
//}

