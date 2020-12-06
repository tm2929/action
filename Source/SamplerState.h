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
			case 0://sprite�p
				//�g��k�����̐F�̎擾���@
				desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;//�k���A�g��Ƀ|�C���g �T���v�����O���g�p���A�~�b�v���x���̃T���v�����O�ɐ��`��Ԃ��g�p���܂��B
				//�e�N�X�`����U���W��0�`1�͈̔͊O�̏ꍇ�̐F�̎擾���@
				//https://www.3dgep.com/texturing-lighting-directx-11/
				desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;//0.0 �܂��� 1.0 �̃e�N�X�`���[ �J���[���ݒ肳��܂��B
				desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
				desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
				desc.MipLODBias = 0;
				desc.MaxAnisotropy = 16;
				desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
				desc.MinLOD = 0;
				desc.MaxLOD = D3D11_FLOAT32_MAX;
				break;
			case 1://bath�p
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
				//�{�[�_�[�J���[
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
