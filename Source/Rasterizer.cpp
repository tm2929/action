#include "Rasterizer.h"
#include "misc.h"
#include "ShaderEX.h"
Rasterizer::Rasterizer(ID3D11Device* device, D3D11_FILTER sampler_filter, D3D11_TEXTURE_ADDRESS_MODE sampler_texture_address_mode, DirectX::XMFLOAT4 sampler_boarder_colour)
{
	HRESULT hr = S_OK;

	vertex vertices[] =
	{
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
	};
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresource_data = {};
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&bufferDesc, &subresource_data, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	static const D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	create_vs_from_cso(device, "Data/shaders/cso/Rasterizer_vs.cso", embeddedVertexShader.ReleaseAndGetAddressOf(), embeddedInputLayout.ReleaseAndGetAddressOf(), const_cast<D3D11_INPUT_ELEMENT_DESC*>(input_element_desc), _countof(input_element_desc));

	create_ps_from_cso(device, "Data/shaders/cso/Rasterizer_ps.cso", embeddedPixelShaders.ReleaseAndGetAddressOf());


	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = sampler_filter; //D3D11_FILTER_MIN_MAG_MIP_POINT D3D11_FILTER_ANISOTROPIC
	samplerDesc.AddressU = sampler_texture_address_mode;
	samplerDesc.AddressV = sampler_texture_address_mode;
	samplerDesc.AddressW = sampler_texture_address_mode;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	memcpy(samplerDesc.BorderColor, &sampler_boarder_colour, sizeof(DirectX::XMFLOAT4));
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&samplerDesc, embeddedSamplerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, embeddedRasterizerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&depthStencilDesc, embeddedDepthStencilState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void Rasterizer::Blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* shaderResourceView, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle, float r, float g, float b, float a, bool useEmbeddedVertexShader, bool useEmbeddedPixelShader, bool useEmbeddedRasterizerState, bool useEmbeddedDepthStencilState, bool useEmbeddedSamplerState) const
{
	HRESULT hr = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceView->GetDesc(&shaderResourceViewDesc);
	

	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	shaderResourceView->GetResource(resource.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_TEXTURE2D_DESC texture2dDesc;
	texture2d->GetDesc(&texture2dDesc);

	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	immediate_context->RSGetViewports(&numViewports, &viewport);
	float screenWidth = viewport.Width;
	float screenHeight = viewport.Height;

	// Set each sprite's vertices coordinate to screen spaceenum BLEND_STATE
		// left-top
	float x0 = dx;
	float y0 = dy;
	// right-top
	float x1 = dx + dw;
	float y1 = dy;
	// left-bottom
	float x2 = dx;
	float y2 = dy + dh;
	// right-bottom
	float x3 = dx + dw;
	float y3 = dy + dh;

	// Translate sprite's centre to origin (rotate centre)
	float mx = dx + dw * 0.5f;
	float my = dy + dh * 0.5f;
	x0 -= mx;
	y0 -= my;
	x1 -= mx;
	y1 -= my;
	x2 -= mx;
	y2 -= my;
	x3 -= mx;
	y3 -= my;

	// Rotate each sprite's vertices by angle
	float rx, ry;
	float cos = cosf(angle * 0.01745f);
	float sin = sinf(angle * 0.01745f);
	rx = x0;
	ry = y0;
	x0 = cos * rx + -sin * ry;
	y0 = sin * rx + cos * ry;
	rx = x1;
	ry = y1;
	x1 = cos * rx + -sin * ry;
	y1 = sin * rx + cos * ry;
	rx = x2;
	ry = y2;
	x2 = cos * rx + -sin * ry;
	y2 = sin * rx + cos * ry;
	rx = x3;
	ry = y3;
	x3 = cos * rx + -sin * ry;
	y3 = sin * rx + cos * ry;

	// Translate sprite's centre to original position
	x0 += mx;
	y0 += my;
	x1 += mx;
	y1 += my;
	x2 += mx;
	y2 += my;
	x3 += mx;
	y3 += my;

	// Convert to NDC space
	x0 = 2.0f * x0 / screenWidth - 1.0f;
	y0 = 1.0f - 2.0f * y0 / screenHeight;
	x1 = 2.0f * x1 / screenWidth - 1.0f;
	y1 = 1.0f - 2.0f * y1 / screenHeight;
	x2 = 2.0f * x2 / screenWidth - 1.0f;
	y2 = 1.0f - 2.0f * y2 / screenHeight;
	x3 = 2.0f * x3 / screenWidth - 1.0f;
	y3 = 1.0f - 2.0f * y3 / screenHeight;

	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;
	hr = immediate_context->Map(vertexBuffer.Get(), 0, map, 0, &mappedBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	vertex* vertices = static_cast<vertex*>(mappedBuffer.pData);
	vertices[0].position.x = x0;
	vertices[0].position.y = y0;
	vertices[1].position.x = x1;
	vertices[1].position.y = y1;
	vertices[2].position.x = x2;
	vertices[2].position.y = y2;
	vertices[3].position.x = x3;
	vertices[3].position.y = y3;
	vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0.0f;

	DirectX::XMFLOAT4 colour(r, g, b, a);
	vertices[0].colour = vertices[1].colour = vertices[2].colour = vertices[3].colour = colour;

	vertices[0].texcoord.x = (sx) / texture2dDesc.Width;
	vertices[0].texcoord.y = (sy) / texture2dDesc.Height;
	vertices[1].texcoord.x = (sx + sw) / texture2dDesc.Width;
	vertices[1].texcoord.y = (sy) / texture2dDesc.Height;
	vertices[2].texcoord.x = (sx) / texture2dDesc.Width;
	vertices[2].texcoord.y = (sy + sh) / texture2dDesc.Height;
	vertices[3].texcoord.x = (sx + sw) / texture2dDesc.Width;
	vertices[3].texcoord.y = (sy + sh) / texture2dDesc.Height;

	immediate_context->Unmap(vertexBuffer.Get(), 0);

	UINT stride = sizeof(vertex);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(embeddedInputLayout.Get());

	if (useEmbeddedVertexShader)
	{
		immediate_context->VSSetShader(embeddedVertexShader.Get(), 0, 0);
	}
	if (useEmbeddedPixelShader)
	{
		immediate_context->PSSetShader(embeddedPixelShaders.Get(), 0, 0);
	}
	immediate_context->PSSetShaderResources(0, 1, &shaderResourceView);

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> default_rasterizer_state;
	if (useEmbeddedRasterizerState)
	{
		immediate_context->RSGetState(default_rasterizer_state.ReleaseAndGetAddressOf());
		immediate_context->RSSetState(embeddedRasterizerState.Get());
	}
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> default_depth_stencil_state;
	if (useEmbeddedDepthStencilState)
	{
		immediate_context->OMGetDepthStencilState(default_depth_stencil_state.ReleaseAndGetAddressOf(), 0);
		immediate_context->OMSetDepthStencilState(embeddedDepthStencilState.Get(), 1);
	}
	Microsoft::WRL::ComPtr<ID3D11SamplerState> default_sampler_state;
	if (useEmbeddedSamplerState)
	{
		immediate_context->PSGetSamplers(0, 1, default_sampler_state.ReleaseAndGetAddressOf());
		immediate_context->PSSetSamplers(0, 1, embeddedSamplerState.GetAddressOf());
	}
	immediate_context->Draw(4, 0);

	immediate_context->IASetInputLayout(0);
	immediate_context->VSSetShader(0, 0, 0);
	immediate_context->PSSetShader(0, 0, 0);

	ID3D11ShaderResourceView* null_shader_resource_view = 0;
	immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);

	if (default_rasterizer_state)
	{
		immediate_context->RSSetState(default_rasterizer_state.Get());
	}
	if (default_depth_stencil_state)
	{
		immediate_context->OMSetDepthStencilState(default_depth_stencil_state.Get(), 1);
	}
	if (default_sampler_state)
	{
		immediate_context->PSSetSamplers(0, 1, default_sampler_state.GetAddressOf());
	}
}


void Rasterizer::Blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* shaderResourceView, float dx, float dy, float dw, float dh, float angle, float r, float g, float b, float a, bool useEmbeddedVertexShader, bool useEmbeddedPixelShader, bool useEmbeddedRasterizerState, bool useEmbeddedDepthStencilState, bool useEmbeddedSamplerState) const
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	shaderResourceView->GetResource(resource.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_TEXTURE2D_DESC texture2d_desc;
	texture2d->GetDesc(&texture2d_desc);

	Blit(immediate_context, shaderResourceView, dx, dy, dw, dh, 0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height), angle, r, g, b, a,
		useEmbeddedVertexShader, useEmbeddedPixelShader, useEmbeddedRasterizerState, useEmbeddedDepthStencilState, useEmbeddedSamplerState);
}

FullscreenQuad::FullscreenQuad(ID3D11Device* device)
{
	HRESULT hr = S_OK;

	create_vs_from_cso(device, "Data/shaders/cso/fullscreen_quad_vs.cso", embeddedVertexShader.ReleaseAndGetAddressOf(), 0, 0, 0,false);

	create_ps_from_cso(device, "Data/shaders/cso/fullscreen_quad_ps.cso", embeddedPixelShaders.ReleaseAndGetAddressOf());

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, embeddedRasterizerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&depthStencilDesc, embeddedDepthStencilState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void FullscreenQuad::Blit(ID3D11DeviceContext* immediate_context, bool useEmbeddedRasterizerState, bool useEmbeddedDepthStencilState, bool useembeddedPixelShader)
{
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> default_rasterizer_state;
	if (useEmbeddedRasterizerState)
	{
		immediate_context->RSGetState(default_rasterizer_state.ReleaseAndGetAddressOf());
		immediate_context->RSSetState(embeddedRasterizerState.Get());
	}

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> default_depth_stencil_state;
	if (useEmbeddedDepthStencilState)
	{
		immediate_context->OMGetDepthStencilState(default_depth_stencil_state.ReleaseAndGetAddressOf(), 0);
		immediate_context->OMSetDepthStencilState(embeddedDepthStencilState.Get(), 1);
	}
	if (useembeddedPixelShader)
	{
		immediate_context->PSSetShader(embeddedPixelShaders.Get(), 0, 0);
	}

	immediate_context->IASetVertexBuffers(0, 0, 0, 0, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(0);

	immediate_context->VSSetShader(embeddedVertexShader.Get(), 0, 0);

	immediate_context->Draw(4, 0);

	//immediate_context->IASetInputLayout(0);
	immediate_context->VSSetShader(0, 0, 0);
	immediate_context->PSSetShader(0, 0, 0);

	if (default_rasterizer_state)
	{
		immediate_context->RSSetState(default_rasterizer_state.Get());
	}
	if (default_depth_stencil_state)
	{
		immediate_context->OMSetDepthStencilState(default_depth_stencil_state.Get(), 1);
	}

}
