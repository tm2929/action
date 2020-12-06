#include <string>
#include "texture.h"
#include <DirectXTex.h>
#include <Shlwapi.h>
#include "misc.h"
#include <map>
#include <mutex>
#include <sstream>
HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* file_name, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;
	auto it = cache.find(file_name);
	if (it != cache.end())
	{
		//it->second.Attach(*shader_resource_view);
		*shader_resource_view = it->second.Get();
		(*shader_resource_view)->AddRef();
		
	}
	else
	{
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;

		std::wstring extension = PathFindExtensionW(file_name);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);
		
		if (L".png" == extension || L".jpeg" == extension || L".jpg" == extension || L".jpe" == extension || L".gif" == extension || L".tiff" == extension || L".tif" == extension || L".bmp" == extension)
		{
			hr = DirectX::LoadFromWICFile(file_name, 0, &metadata, image);
			//_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		else if (L".dds" == extension)
		{
			hr = DirectX::LoadFromDDSFile(file_name, 0, &metadata, image);
			//_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		else if (L".tga" == extension || L".vda" == extension || L".icb" == extension || L".vst" == extension)
		{
			hr = DirectX::LoadFromTGAFile(file_name, &metadata, image);
			//_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		else
		{
			_ASSERT_EXPR(false, L"File format not supported.");
		}
		if (FAILED(hr)) return hr;
		hr = DirectX::CreateShaderResourceViewEx(
			device,
			image.GetImages(),
			image.GetImageCount(),
			metadata,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			D3D11_RESOURCE_MISC_TEXTURECUBE,
			false,
			shader_resource_view);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		cache.insert(std::make_pair(file_name, *shader_resource_view));

	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	(*shader_resource_view)->GetResource(resource.GetAddressOf());
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	texture2d->GetDesc(texture2d_desc);

	return hr;
}

bool Texture::Load(ID3D11Device* device, const wchar_t* filename)
{
	HRESULT hr = S_OK;

	hr = load_texture_from_file(device, filename, shaderResouceView.GetAddressOf(),&texture2dDesc);
	samplerState = std::make_unique<Sampler>(device, 0);

	return true;
}

bool Texture::Dummy(ID3D11Device* device)
{
	HRESULT hr = S_OK;

	texture2dDesc.Width = 1;
	texture2dDesc.Height = 1;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.MiscFlags = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DYNAMIC;
	texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2dDesc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> tex2d = nullptr;
	hr = device->CreateTexture2D(&texture2dDesc, nullptr, tex2d.GetAddressOf());
	if (FAILED(hr))
	{
		assert(!"CreateTexture2D");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = texture2dDesc.Format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MostDetailedMip = 0;
	desc.Texture2D.MipLevels = texture2dDesc.MipLevels;
	hr = device->CreateShaderResourceView(tex2d.Get(), &desc, shaderResouceView.GetAddressOf());
	if (FAILED(hr))
	{
		assert(!"shader_resource_view");
	}

	return true;
}

void Texture::Set(ID3D11DeviceContext* context, UINT Slot, BOOL flg)
{
	if (!flg)
	{
		ID3D11ShaderResourceView* srv[1] = { nullptr };
		ID3D11SamplerState* ss[1] = { nullptr };
		context->PSSetShaderResources(Slot, 1, srv);
		context->PSSetSamplers(Slot, 1, ss);
		context->DSSetShaderResources(Slot, 1, srv);
		context->DSSetSamplers(Slot, 1, ss);
	}
	if (shaderResouceView)
	{
		context->PSSetShaderResources(Slot, 1, shaderResouceView.GetAddressOf());
		context->DSSetShaderResources(Slot, 1, shaderResouceView.GetAddressOf());

		samplerState->Activate(context, Slot);
	}
}

bool Texture::Create(ID3D11Device* device, u_int width, u_int height, DXGI_FORMAT format)
{
	ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2dDesc, sizeof(texture2dDesc));
	texture2dDesc.Width = width;
	texture2dDesc.Height = height;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = format;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2dDesc, NULL, Texture2D.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, renderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}


	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, shaderResouceView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}


	samplerState = std::make_unique<Sampler>(device, 2);

	return true;
}

bool Texture::CreateCube(ID3D11Device* device,u_int width, u_int height, int miplevel)
{
	
	ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2dDesc, sizeof(texture2dDesc));
	texture2dDesc.Width = width;
	texture2dDesc.Height = height;
	texture2dDesc.MipLevels = miplevel;//★
	texture2dDesc.ArraySize = 6;
	texture2dDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	//	texture2d_desc.CPUAccessFlags = 0;
	texture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	texture2dDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = device->CreateTexture2D(&texture2dDesc, NULL, Texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;//2Dテクスチャ配列
	rtvd.Texture2DArray.FirstArraySlice = 0;
	rtvd.Texture2DArray.ArraySize = 6;
	rtvd.Texture2DArray.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, renderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvd.TextureCube.MostDetailedMip = 0;
	srvd.TextureCube.MipLevels = miplevel;
	//srvd.TextureCube.MipLevels = 0;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, shaderResouceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	return true;
}

bool Texture::CreateCubeDepthStencil(ID3D11Device* device,u_int width, u_int height)
{
	ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2dDesc, sizeof(texture2dDesc));
	texture2dDesc.Width = width;
	texture2dDesc.Height = height;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 6;
	texture2dDesc.Format = DXGI_FORMAT_D32_FLOAT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.CPUAccessFlags = 0;
	//	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2dDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = device->CreateTexture2D(&texture2dDesc, NULL, Texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;//2Dテクスチャ配列
	dsvd.Texture2DArray.FirstArraySlice = 0;
	dsvd.Texture2DArray.ArraySize = 6;
	dsvd.Texture2DArray.MipSlice = 0;
	dsvd.Flags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	hr = device->CreateDepthStencilView(Texture2D.Get(), &dsvd, depthStencilView.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}

