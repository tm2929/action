#include "framebuffer.h"
#include"misc.h"

FrameBuffer::FrameBuffer(ID3D11Device* device, int width, int height, bool msaaFlag, int subsamples, DXGI_FORMAT renderTargetTexture2dFormat, DXGI_FORMAT depthStencilTexture2dFormat,
	bool renderTargetShaderResourceViewFlag, bool depthStencilShaderResourceViewFlag, bool mipsFlag)
{
	HRESULT hr = S_OK;
	UINT qualityLevel;
	UINT sampleCount = subsamples;
	hr = device->CheckMultisampleQualityLevels(renderTargetTexture2dFormat, sampleCount, &qualityLevel);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	if (renderTargetTexture2dFormat != DXGI_FORMAT_UNKNOWN)
	{
		D3D11_TEXTURE2D_DESC texture2d_desc = {};
		texture2d_desc.Width = width;
		texture2d_desc.Height = height;
		texture2d_desc.MipLevels = mipsFlag ? 0 : 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = renderTargetTexture2dFormat;
		texture2d_desc.SampleDesc.Count = msaaFlag ? sampleCount : 1;
		texture2d_desc.SampleDesc.Quality = msaaFlag ? qualityLevel - 1 : 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = renderTargetShaderResourceViewFlag ? D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_RENDER_TARGET;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = mipsFlag ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = device->CreateTexture2D(&texture2d_desc, 0, texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc = {};
		render_target_view_desc.Format = texture2d_desc.Format;
		render_target_view_desc.ViewDimension = msaaFlag ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = device->CreateRenderTargetView(texture2d.Get(), &render_target_view_desc, mRenderTargetView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		if (renderTargetShaderResourceViewFlag)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
			shader_resource_view_desc.Format = texture2d_desc.Format;
			shader_resource_view_desc.ViewDimension = msaaFlag ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			shader_resource_view_desc.Texture2D.MipLevels = mipsFlag ? -1 : 1;
			hr = device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc, mRenderTargetShaderResourceView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

	}
	if (depthStencilTexture2dFormat != DXGI_FORMAT_UNKNOWN)
	{
		const DXGI_FORMAT combinations_of_depth_stencil_formats[3][3] =
		{
			{ DXGI_FORMAT_R24G8_TYPELESS,  DXGI_FORMAT_D24_UNORM_S8_UINT ,DXGI_FORMAT_R24_UNORM_X8_TYPELESS },
			{ DXGI_FORMAT_R32_TYPELESS,  DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_R32_FLOAT },
			{ DXGI_FORMAT_R16_TYPELESS,  DXGI_FORMAT_D16_UNORM , DXGI_FORMAT_R16_UNORM },
		};
		int depth_stencil_texture2d_format_index = 0;
		switch (depthStencilTexture2dFormat)
		{
		case DXGI_FORMAT_R24G8_TYPELESS:
			depth_stencil_texture2d_format_index = 0;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
			depth_stencil_texture2d_format_index = 1;
			break;
		case DXGI_FORMAT_R16_TYPELESS:
			depth_stencil_texture2d_format_index = 2;
			break;
		}
		D3D11_TEXTURE2D_DESC texture2d_desc = {};
		texture2d_desc.Width = width;
		texture2d_desc.Height = height;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = combinations_of_depth_stencil_formats[depth_stencil_texture2d_format_index][0];
		texture2d_desc.SampleDesc.Count = msaaFlag ? sampleCount : 1;
		texture2d_desc.SampleDesc.Quality = msaaFlag ? qualityLevel - 1 : 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = depthStencilShaderResourceViewFlag ? D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_DEPTH_STENCIL;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = device->CreateTexture2D(&texture2d_desc, 0, texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
		depth_stencil_view_desc.Format = combinations_of_depth_stencil_formats[depth_stencil_texture2d_format_index][1];
		depth_stencil_view_desc.ViewDimension = msaaFlag ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Flags = 0;
		hr = device->CreateDepthStencilView(texture2d.Get(), &depth_stencil_view_desc, mDepthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		if (depthStencilShaderResourceViewFlag)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
			shader_resource_view_desc.Format = combinations_of_depth_stencil_formats[depth_stencil_texture2d_format_index][2];
			shader_resource_view_desc.ViewDimension = msaaFlag ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			shader_resource_view_desc.Texture2D.MipLevels = 1;
			hr = device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc, mDepthStencilShaderResourceView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
	}
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
}

void FrameBuffer::Clear(ID3D11DeviceContext* context, float r, float g, float b, float a, unsigned int clearFlags, float depth, unsigned char stencil)
{
	float color[4] = { r,g,b,a };
	if (mRenderTargetView)
	{
		context->ClearRenderTargetView(mRenderTargetView.Get(), color);
	}
	if (mDepthStencilView)
	{
		context->ClearDepthStencilView(mDepthStencilView.Get(), clearFlags, depth, stencil);
	}
}

void FrameBuffer::Activate(ID3D11DeviceContext* context)
{
	viewportsNumber = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	context->RSGetViewports(&viewportsNumber, defaultViewports);
	context->RSSetViewports(1, &viewport);

	context->OMGetRenderTargets(1, mDefaultRenderTargetView.GetAddressOf(), mDefaultDepthStencilView.GetAddressOf());
	context->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
}

void FrameBuffer::Deactivate(ID3D11DeviceContext* context)
{
	context->RSSetViewports(viewportsNumber, defaultViewports);
	context->OMSetRenderTargets(1, mDefaultRenderTargetView.GetAddressOf(), mDefaultDepthStencilView.Get());

}