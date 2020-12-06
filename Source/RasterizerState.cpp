
#include "RasterizerState.h"
#include <assert.h>

HRESULT RasterizerState::CreateRasterizerState(ID3D11Device* device)
{
    HRESULT hr = S_OK;
    now_rasteriz = -1;

    D3D11_RASTERIZER_DESC rd;
    for (int state = 0; state < RASTERIZE_TYPE; state++) {
        switch (state) {
        case RS_CULL_BACK:
            ZeroMemory(&rd, sizeof(rd));
            rd.FillMode = D3D11_FILL_SOLID;
            rd.CullMode = D3D11_CULL_BACK;
            rd.FrontCounterClockwise = FALSE;
            rd.DepthBias = 0;
            rd.DepthBiasClamp = 0;
            rd.SlopeScaledDepthBias = 0;
            rd.DepthClipEnable = TRUE;
            rd.ScissorEnable = FALSE;
            rd.MultisampleEnable = TRUE;
            rd.AntialiasedLineEnable = FALSE;

            break;

        case RS_WIRE:
            ZeroMemory(&rd, sizeof(rd));
            rd.FillMode = D3D11_FILL_WIREFRAME;
            rd.CullMode = D3D11_CULL_BACK;
            rd.FrontCounterClockwise = FALSE;
            rd.DepthBias = 0;
            rd.DepthBiasClamp = 0;
            rd.SlopeScaledDepthBias = 0;
            rd.DepthClipEnable = TRUE;
            rd.ScissorEnable = FALSE;
            rd.MultisampleEnable = FALSE;
            rd.AntialiasedLineEnable = FALSE;
            break;

        case RS_CULL_FRONT:
            ZeroMemory(&rd, sizeof(rd));
            rd.FillMode = D3D11_FILL_SOLID;
            rd.CullMode = D3D11_CULL_FRONT;
            rd.FrontCounterClockwise = FALSE;
            rd.DepthBias = 0;
            rd.DepthBiasClamp = 0;
            rd.SlopeScaledDepthBias = 0;
            rd.DepthClipEnable = TRUE;
            rd.ScissorEnable = FALSE;
            rd.MultisampleEnable = FALSE;
            rd.AntialiasedLineEnable = FALSE;

            break;

        case RS_CULL_NONE:
            ZeroMemory(&rd, sizeof(rd));
            rd.FillMode = D3D11_FILL_SOLID;
            rd.CullMode = D3D11_CULL_NONE;
            rd.FrontCounterClockwise = FALSE;
            rd.DepthBias = 0;
            rd.DepthBiasClamp = 0;
            rd.SlopeScaledDepthBias = 0;
            rd.DepthClipEnable = TRUE;
            rd.ScissorEnable = FALSE;
            rd.MultisampleEnable = FALSE;
            rd.AntialiasedLineEnable = FALSE;

            break;

        case RS_CULL_BACK_CLOCK:
            rd.FillMode = D3D11_FILL_SOLID;
            rd.CullMode = D3D11_CULL_BACK;
            rd.FrontCounterClockwise = TRUE;
            rd.DepthBias = 0;
            rd.DepthBiasClamp = 0;
            rd.SlopeScaledDepthBias = 0;
            rd.DepthClipEnable = TRUE;
            rd.ScissorEnable = FALSE;
            rd.MultisampleEnable = TRUE;
            rd.AntialiasedLineEnable = FALSE;

            break;
        }
        hr = device->CreateRasterizerState(
            &rd, rasterizer_state[state].GetAddressOf());

        if (FAILED(hr))
        {
            assert(!"CreateRasterizerState");
            return hr;
        }
    }

    return hr;
}

void RasterizerState::SetRasterizerState(const int rasterizMode, ID3D11DeviceContext* context)
{
    if (0 > rasterizMode || rasterizMode > RASTERIZE_TYPE) return;
    if (rasterizMode == now_rasteriz) return;

    context->RSSetState(rasterizer_state[rasterizMode].Get());
    now_rasteriz = rasterizMode;
}
