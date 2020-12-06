#include"sprite.h"
#include<stdio.h>
#include "ShaderEX.h"
#include "Texture.h"
#include "misc.h"
Sprite::Sprite(ID3D11Device* Device, const wchar_t* Texture_file_name/*Texture file name*/)
{
	HRESULT hr;
	vertex vertices[] = {
		{ DirectX::XMFLOAT3(-0.5, +0.5, 0), DirectX::XMFLOAT4(1, 1, 1, 1) , DirectX::XMFLOAT2(0.0f, 0.0f) },
		{ DirectX::XMFLOAT3(+0.5, +0.5, 0), DirectX::XMFLOAT4(1, 0, 0, 1) , DirectX::XMFLOAT2(1.0f, 0.0f) },
		{ DirectX::XMFLOAT3(-0.5, -0.5, 0), DirectX::XMFLOAT4(0, 1, 0, 1) , DirectX::XMFLOAT2(0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(+0.5, -0.5, 0), DirectX::XMFLOAT4(0, 0, 1, 1) , DirectX::XMFLOAT2(1.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC structure = {};
	ZeroMemory(&structure, sizeof(structure));
	structure.Usage = D3D11_USAGE_DYNAMIC;
	structure.ByteWidth = sizeof(vertex) * 4;
	structure.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	structure.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA structure_data;
	ZeroMemory(&structure_data, sizeof(structure_data));
	structure_data.pSysMem = vertices;
	//virtual HRESULT STDMETHODCALLTYPE CreateBuffer(
	//	/* [annotation] */
	//	_In_  const D3D11_BUFFER_DESC *pDesc,  引数
	//	/* [annotation] */
	//	_In_opt_  const D3D11_SUBRESOURCE_DATA *pInitialData,  引数
	//	/* [annotation] */
	//	_Out_opt_  ID3D11Buffer **ppBuffer) = 0;  戻り値

	hr = Device->CreateBuffer(&structure, &structure_data, &buffer);
	if (FAILED(hr))
	{
		assert(0);
	}

	D3D11_INPUT_ELEMENT_DESC input_element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(input_element);

	hr = create_vs_from_cso(Device, "Data/shaders/cso/sprite_vs.cso", &vertexshader, &input, input_element, numElements);
	if (FAILED(hr))
	{
		assert(0);
	}
	hr = create_ps_from_cso(Device, "Data/shaders/cso/sprite_ps.cso", &pixe);
	if (FAILED(hr))
	{
		assert(0);
	}


	//D3D11_RASTERIZER_DESC rasterizer_desc;
	//typedef struct D3D11_RASTERIZER_DESC
	//{
	// D3D11_FILL_MODE FillMode;
	// D3D11_CULL_MODE CullMode;
	// BOOL FrontCounterClockwise;
	// INT DepthBias;
	// FLOAT DepthBiasClamp;
	// FLOAT SlopeScaledDepthBias;
	// BOOL DepthClipEnable;
	// BOOL ScissorEnable;
	// BOOL MultisampleEnable;
	// BOOL AntialiasedLineEnable;
	//} 	D3D11_RASTERIZER_DESC;

	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthBias = 0;
	noCullDesc.DepthBiasClamp = 0.0f;
	noCullDesc.SlopeScaledDepthBias = 0.0f;
	noCullDesc.DepthClipEnable = true;
	noCullDesc.ScissorEnable = false;
	noCullDesc.MultisampleEnable = true;
	noCullDesc.AntialiasedLineEnable = false;




	/// rasterizer_desc.CullMode


	//D3D11_RASTERIZER_DESC rasterizer_desc = {
	// D3D11_FILL_SOLID,
	// D3D11_CULL_NONE,	//ポリゴンの裏表を無くす
	// false,
	// 0,
	// 1.0f,
	// true,
	// true,
	// true,
	// true,
	// false
	//};

	hr = Device->CreateRasterizerState(&noCullDesc, &rasterrizer);
	if (FAILED(hr))
	{
		assert(0);
	}
	//UNIT05


	hr = load_texture_from_file(Device, Texture_file_name, &srview, &texture2d);
	if (FAILED(hr)) {
		assert(0);
	}

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = -FLT_MAX;
	sampDesc.MaxLOD = FLT_MAX;
	sampDesc.MaxAnisotropy = 16;
	memcpy(sampDesc.BorderColor, &DirectX::XMFLOAT4(1.0f, 10.f, 10.f, 10.f), sizeof(DirectX::XMFLOAT4));
	hr = Device->CreateSamplerState(&sampDesc, &samplerstate);
	if (FAILED(hr)) {
		assert(0);
	}
	//UNIT06
	D3D11_DEPTH_STENCIL_DESC depthsteDesc;
	ZeroMemory(&depthsteDesc, sizeof(depthsteDesc));
	depthsteDesc.DepthEnable = false;
	depthsteDesc.StencilEnable = false;
	depthsteDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthsteDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthsteDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthsteDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthsteDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	depthsteDesc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;
	depthsteDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthsteDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthsteDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	depthsteDesc.BackFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;

	//virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilState(
	// /* [annotation] */
	// _In_  const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
	// /* [annotation] */
	// _Out_opt_  ID3D11DepthStencilState **ppDepthStencilState) = 0;

	hr = Device->CreateDepthStencilState(&depthsteDesc, &depthstencil);
	if (FAILED(hr)) {
		assert(0);
	}
	// D3D11_BLEND_DESC blDesc;
	// ZeroMemory(&blDesc, sizeof(blDesc));
	// blDesc.RenderTarget[0].BlendEnable = true;
	// //画像の色
	// blDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	// blDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	// blDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;//画像同士の色の計算
	////アルファ値(固定?)
	// blDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;//COLOR(1,1,1,1)   :今回描画されるもの
	// blDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;//COLOR(0,0,0,0) :既に描画されてるもの
	// blDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;//アルファ値の計算

	// blDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	// //virtual HRESULT STDMETHODCALLTYPE CreateBlendState(
	//	// /* [annotation] */
	//	// _In_  const D3D11_BLEND_DESC *pBlendStateDesc,
	//	// /* [annotation] */
	//	// _Out_opt_  ID3D11BlendState **ppBlendState) = 0;

	// hr = Device->CreateBlendState(&blDesc, &blend);
	// if (FAILED(hr)) {
	//	 assert(0);
	// }

}
void Sprite::Render(ID3D11DeviceContext* devicecontext,
	float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle, float r, float g, float b, float a)
{
	UINT pstrides = sizeof(vertex);
	UINT poff = 0;

	HRESULT hr;
	D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped;

	//セット
	vertex vertices2[] = {
		{ DirectX::XMFLOAT3(dx, dy, 0.0f), DirectX::XMFLOAT4(r, g, b, a) ,DirectX::XMFLOAT2(sx,sy) },
		{ DirectX::XMFLOAT3(dx + dw, dy, 0.0f), DirectX::XMFLOAT4(r, g, b, a),DirectX::XMFLOAT2(sx + sw,sy) },
		{ DirectX::XMFLOAT3(dx, dy + dh, 0.0f), DirectX::XMFLOAT4(r, g, b, a) ,DirectX::XMFLOAT2(sx,sy + sh) },
		{ DirectX::XMFLOAT3(dx + dw, dy + dh, 0.0f), DirectX::XMFLOAT4(r, g, b, a) ,DirectX::XMFLOAT2(sx + sw,sy + sh) },
	};
	//		回転軸の変更(短形中央へ)
	for (int i = 0; i < 4; i++) {
		vertices2[i].position.x -= dx + dw / 2.0f;
		vertices2[i].position.y -= dy + dh / 2.0f;
	}
	//      回転
	float degree = DirectX::XMConvertToRadians(angle);
	float si = sinf(degree);
	float co = cosf(degree);

	for (int i = 0; i < 4; i++) {
		float pos_x = vertices2[i].position.x;
		float pos_y = vertices2[i].position.y;
		vertices2[i].position.x = pos_x * co - pos_y * si;
		vertices2[i].position.y = pos_x * si + pos_y * co;
	}
	//		回転軸の変更(元の位置へ)
	for (int i = 0; i < 4; i++) {
		vertices2[i].position.x += dx + dw / 2.0f;
		vertices2[i].position.y += dy + dh / 2.0f;
	}


	//virtual HRESULT STDMETHODCALLTYPE Map(
	//	/* [annotation] */
	//	_In_  ID3D11Resource *pResource,
	//	/* [annotation] */
	//	_In_  UINT Subresource,
	//	/* [annotation] */
	//	_In_  D3D11_MAP MapType,
	//	/* [annotation] */
	//	_In_  UINT MapFlags,
	//	/* [annotation] */
	//	_Out_  D3D11_MAPPED_SUBRESOURCE *pMappedResource) = 0;


	hr = devicecontext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	if (FAILED(hr)) {
		assert(0);
	};
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	devicecontext->RSGetViewports(&num_viewports, &viewport);

	vertex* v = (vertex*)mapped.pData;

	for (int i = 0; i < 4; i++) {
		v[i].position.x = (vertices2[i].position.x / viewport.Width) * 2.0f - 1.0f;
		v[i].position.y = -(vertices2[i].position.y / viewport.Height) * 2.0f + 1.0f;
		v[i].color = vertices2[i].color;
		v[i].texcoord.x = vertices2[i].texcoord.x / texture2d.Width;
		v[i].texcoord.y = vertices2[i].texcoord.y / texture2d.Height;
	}

	devicecontext->Unmap(buffer, 0);



	devicecontext->IASetVertexBuffers(0, 1, &buffer, &pstrides, &poff);

	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	devicecontext->IASetInputLayout(input);

	devicecontext->RSSetState(rasterrizer);

	devicecontext->VSSetShader(vertexshader, NULL, 0);

	devicecontext->PSSetShader(pixe, NULL, 0);
	//virtual void STDMETHODCALLTYPE PSSetShaderResources(
	//	/* [annotation] */
	//	_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
	//	/* [annotation] */
	//	_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
	//	/* [annotation] */
	//	_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews) = 0;
	devicecontext->PSSetShaderResources(0, 1, &srview);

	devicecontext->PSSetSamplers(0, 1, &samplerstate);

	devicecontext->OMSetDepthStencilState(depthstencil, 0);

	//virtual void STDMETHODCALLTYPE OMSetBlendState(
	//	/* [annotation] */
	//	_In_opt_  ID3D11BlendState *pBlendState,
	//	/* [annotation] */
	//	_In_opt_  const FLOAT BlendFactor[4],
	//	/* [annotation] */
	//	_In_  UINT SampleMask) = 0;
	//devicecontext->OMSetBlendState(blend, NULL,0xffffff);

	devicecontext->Draw(4, 0);

}

void Sprite::Render(ID3D11DeviceContext* devicecontext, ID3D11ShaderResourceView* srv,
	float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle, float r, float g, float b, float a)
{
	UINT pstrides = sizeof(vertex);
	UINT poff = 0;


	HRESULT hr;
	D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	srv->GetResource(resource.GetAddressOf());
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(tex.GetAddressOf());

	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	D3D11_TEXTURE2D_DESC tex2d;
	tex->GetDesc(&tex2d);
	//セット
	vertex vertices2[] = {
		{ DirectX::XMFLOAT3(dx, dy, 0.0f), DirectX::XMFLOAT4(r, g, b, a) ,DirectX::XMFLOAT2(sx,sy) },
		{ DirectX::XMFLOAT3(dx + dw, dy, 0.0f), DirectX::XMFLOAT4(r, g, b, a),DirectX::XMFLOAT2(sx + sw,sy) },
		{ DirectX::XMFLOAT3(dx, dy + dh, 0.0f), DirectX::XMFLOAT4(r, g, b, a) ,DirectX::XMFLOAT2(sx,sy + sh) },
		{ DirectX::XMFLOAT3(dx + dw, dy + dh, 0.0f), DirectX::XMFLOAT4(r, g, b, a) ,DirectX::XMFLOAT2(sx + sw,sy + sh) },
	};
	//		回転軸の変更(短形中央へ)
	for (int i = 0; i < 4; i++) {
		vertices2[i].position.x -= dx + dw / 2.0f;
		vertices2[i].position.y -= dy + dh / 2.0f;
	}
	//      回転
	float degree = DirectX::XMConvertToRadians(angle);
	float si = sinf(degree);
	float co = cosf(degree);

	for (int i = 0; i < 4; i++) {
		float pos_x = vertices2[i].position.x;
		float pos_y = vertices2[i].position.y;
		vertices2[i].position.x = pos_x * co - pos_y * si;
		vertices2[i].position.y = pos_x * si + pos_y * co;
	}
	//		回転軸の変更(元の位置へ)
	for (int i = 0; i < 4; i++) {
		vertices2[i].position.x += dx + dw / 2.0f;
		vertices2[i].position.y += dy + dh / 2.0f;
	}
	hr = devicecontext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	if (FAILED(hr)) {
		assert(0);
	};
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	devicecontext->RSGetViewports(&num_viewports, &viewport);

	vertex* v = (vertex*)mapped.pData;

	for (int i = 0; i < 4; i++) {
		v[i].position.x = (vertices2[i].position.x / viewport.Width) * 2.0f - 1.0f;
		v[i].position.y = -(vertices2[i].position.y / viewport.Height) * 2.0f + 1.0f;
		v[i].color = vertices2[i].color;
		v[i].texcoord.x = vertices2[i].texcoord.x / tex2d.Width;
		v[i].texcoord.y = vertices2[i].texcoord.y / tex2d.Height;
	}

	devicecontext->Unmap(buffer, 0);



	devicecontext->IASetVertexBuffers(0, 1, &buffer, &pstrides, &poff);

	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	devicecontext->IASetInputLayout(input);

	devicecontext->RSSetState(rasterrizer);

	devicecontext->VSSetShader(vertexshader, NULL, 0);

	devicecontext->PSSetShader(pixe, NULL, 0);
	//virtual void STDMETHODCALLTYPE PSSetShaderResources(
	//	/* [annotation] */
	//	_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
	//	/* [annotation] */
	//	_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
	//	/* [annotation] */
	//	_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews) = 0;
	devicecontext->PSSetShaderResources(0, 1, &srv);

	devicecontext->PSSetSamplers(0, 1, &samplerstate);

	devicecontext->OMSetDepthStencilState(depthstencil, 0);

	//virtual void STDMETHODCALLTYPE OMSetBlendState(
	//	/* [annotation] */
	//	_In_opt_  ID3D11BlendState *pBlendState,
	//	/* [annotation] */
	//	_In_opt_  const FLOAT BlendFactor[4],
	//	/* [annotation] */
	//	_In_  UINT SampleMask) = 0;
	//devicecontext->OMSetBlendState(blend, NULL,0xffffff);

	devicecontext->Draw(4, 0);
}

//Sprite::~Sprite() 
//{
//		if (vertexshader)vertexshader->Release();
//		if (pixe)pixe->Release();
//		if (input)input->Release();
//		if (buffer)buffer->Release();
//		if (rasterrizer)rasterrizer->Release();
//		if (srview)srview->Release();
//		if (samplerstate)samplerstate->Release();
//}
