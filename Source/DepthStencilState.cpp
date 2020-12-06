
#include "DepthStencilState.h"
#include <assert.h>

HRESULT DepthStencilState::CreateDepthStencilState(ID3D11Device* device)
{
	HRESULT hr = S_OK;

	D3D11_DEPTH_STENCIL_DESC dsd;
	ZeroMemory(&dsd, sizeof(dsd));
	dsd.DepthEnable = FALSE;
	dsd.StencilEnable = FALSE;
	hr = device->CreateDepthStencilState(&dsd, depth_stencil_state[DS_FALSE].GetAddressOf());

	if (FAILED(hr))
	{
		assert(!"CreateDepthStencilState FALSE");
	}

	ZeroMemory(&dsd, sizeof(dsd));
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	hr = device->CreateDepthStencilState(&dsd, depth_stencil_state[DS_TRUE].GetAddressOf());

	if (FAILED(hr))
	{
		assert(!"CreateDepthStencilState TRUE");
	}
	return hr;
}

void DepthStencilState::SetDepthStencilState(const int flg, ID3D11DeviceContext* context)
{
	if (0 > flg || flg > 2) return;
	if (flg == now_depth) return;
	
	context->OMSetDepthStencilState(depth_stencil_state[flg].Get(), 0);
	now_depth = flg;
}
