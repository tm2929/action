#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <assert.h>

template <class T>
struct ConstantBuffer
{
    T data;
    ConstantBuffer(ID3D11Device* device)
    {
        HRESULT hr = S_OK;

        //if (sizeof(T) % 16 == 0)
        //{
        //    assert(!"constant buffer's need to be 16 byte aligned ");
        //}
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.ByteWidth = sizeof(T);
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;

        hr = device->CreateBuffer(&bd, 0, buffer_object.GetAddressOf());
        if (FAILED(hr))
        {
            assert(!"CreateBuffer constant");
        }
    }

    virtual ~ConstantBuffer() = default;
    ConstantBuffer(ConstantBuffer&) = delete;
    ConstantBuffer& operator =(ConstantBuffer&) = delete;

    void Activate(ID3D11DeviceContext* context, int slot, bool set_in_vs = true, bool set_in_ps = true)
    {
        HRESULT hr = S_OK;

        D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
        D3D11_MAPPED_SUBRESOURCE mapped_buffer;

        hr = context->Map(buffer_object.Get(), 0, map, 0, &mapped_buffer);
        if (FAILED(hr))
        {
            assert(!"Map constantbuffer");
        }
        memcpy_s(mapped_buffer.pData, sizeof(T), &data, sizeof(T));
        context->Unmap(buffer_object.Get(), 0);

        using_slot = slot;

        ID3D11Buffer* null_buffer = 0;
        set_in_vs ? context->VSSetConstantBuffers(using_slot, 1, buffer_object.GetAddressOf()) : context->VSSetConstantBuffers(slot, 1, &null_buffer);
        set_in_ps ? context->PSSetConstantBuffers(using_slot, 1, buffer_object.GetAddressOf()) : context->PSSetConstantBuffers(slot, 1, &null_buffer);
    }

    void DeActivate(ID3D11DeviceContext* context)
    {
        if (using_slot < 10)
        {
            ID3D11Buffer* null_buffer = 0;
            context->VSSetConstantBuffers(using_slot, 1, &null_buffer);
            context->PSSetConstantBuffers(using_slot, 1, &null_buffer);
        }
    }
  
private:
    unsigned int using_slot = -1;
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_object;
};