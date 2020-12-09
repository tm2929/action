#pragma once

#include <d3d11.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include "Sampler.h"
#include <memory>
HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* file_name, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);
class Texture
{
public:
    Texture() : shaderResouceView(nullptr), samplerState(nullptr)
    { }
    virtual ~Texture() { };
    bool Load(ID3D11Device* device, const wchar_t* filename = nullptr);
    bool LoadMipMap(ID3D11Device* device, const wchar_t* filename);
    bool Dummy(ID3D11Device* device);
    void Set(ID3D11DeviceContext* context, UINT Slot = 0, BOOL flg = true);
    UINT GetWidth() { return texture2dDesc.Width; }
    UINT GetHeight() { return texture2dDesc.Height; }

    bool Create(ID3D11Device* device, u_int width, u_int height, DXGI_FORMAT format);

    ID3D11RenderTargetView* GetRenderTarget() { return renderTargetView.Get(); }

    //CubeMap—p
    bool CreateCube(ID3D11Device* device, u_int width, u_int height, int miplevel);
    bool CreateCubeDepthStencil(ID3D11Device* device, u_int width, u_int height);
protected:
    ComPtr<ID3D11ShaderResourceView> shaderResouceView;
    std::unique_ptr<Sampler> samplerState;
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    D3D11_TEXTURE2D_DESC texture2dDesc = {};
    ComPtr<ID3D11DepthStencilView> depthStencilView = nullptr;

};
