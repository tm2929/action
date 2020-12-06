#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <assert.h>

using Microsoft::WRL::ComPtr;

enum DEPTH_LAVEL { DS_TRUE, DS_FALSE };
class DepthStencilState
{
	int now_depth = 0;
	ID3D11DeviceContext* keep_context = nullptr;
public:
	ComPtr<ID3D11DepthStencilState> depth_stencil_state[2];

	DepthStencilState() { }
	virtual ~DepthStencilState() { }

	HRESULT CreateDepthStencilState(ID3D11Device* device);

	// 2D -> FALSE 3D -> TRUE
	void SetDepthStencilState(const int flg, ID3D11DeviceContext* context = nullptr);

	ID3D11DepthStencilState* GetDepthStencilState(int num)
	{
		if (num > 2) assert(!"Not Found");
		return depth_stencil_state[num].Get();
	}
};