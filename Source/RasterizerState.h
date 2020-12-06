#pragma once
#pragma once

#include <d3d11.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

enum RASTERIZ_LAVEL { RS_CULL_BACK, RS_WIRE, RS_CULL_FRONT, RS_CULL_NONE, RS_CULL_BACK_CLOCK, MAX };

class RasterizerState
{
    int now_rasteriz = 0;
    ID3D11DeviceContext* keep_context = nullptr;
public:
    static const int RASTERIZE_TYPE = RASTERIZ_LAVEL::MAX;
    ComPtr<ID3D11RasterizerState> rasterizer_state[RASTERIZE_TYPE];

    RasterizerState() { }
    virtual ~RasterizerState() { }

    HRESULT CreateRasterizerState(ID3D11Device* device);
    void SetRasterizerState(const int rasterizMode, ID3D11DeviceContext* context = nullptr);
};

