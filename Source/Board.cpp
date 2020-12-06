#include "Board.h"
#include "ShaderEX.h"
#include "Texture.h"
Board::Board(ID3D11Device* device, const wchar_t* file_name)
{
	HRESULT hr;
	vertex vertices[] =
	{
		{ DirectX::XMFLOAT3(-0.5f, +0.5f, 0.0f), DirectX::XMFLOAT2(1, 0),DirectX::XMFLOAT4(1.f, 1.f, 1.f,1.f) },
		{ DirectX::XMFLOAT3(+0.5f, +0.5f, 0.0f), DirectX::XMFLOAT2(0, 0),DirectX::XMFLOAT4(1.f, 1.f, 1.f,1.f) },
		{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(1, 1),DirectX::XMFLOAT4(1.f, 1.f, 1.f,1.f) },
		{ DirectX::XMFLOAT3(+0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0, 1),DirectX::XMFLOAT4(1.f, 1.f, 1.f,1.f) },
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
	hr = device->CreateBuffer(&structure, &structure_data, &buffer);
	if (FAILED(hr))
	{
		assert(0);
	}
	// 定数バッファを作成するための設定オプション
	D3D11_BUFFER_DESC buffer_desc = {};
	buffer_desc.ByteWidth = sizeof(cbuffer);	// バッファ（データを格納する入れ物）のサイズ
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 定数バッファとしてバッファを作成する。
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	hr = device->CreateBuffer(&buffer_desc, nullptr, &constantBuffer);
	if (FAILED(hr))
	{
		assert(0);
	}

	//シェーダー
	D3D11_INPUT_ELEMENT_DESC input_element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(input_element);

	hr = create_vs_from_cso(device, "Data/shaders/cso/board_vs.cso", &vertexShader, &input, input_element,numElements);
	if (FAILED(hr))
	{
		assert(0);
	}
	hr = create_ps_from_cso(device, "Data/shaders/cso/board_ps.cso", &pixelShader);
	if (FAILED(hr))
	{
		assert(0);
	}

	//
	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = true;
	noCullDesc.DepthBias = 0;
	noCullDesc.DepthBiasClamp = 0.0f;
	noCullDesc.SlopeScaledDepthBias = 0.0f;
	noCullDesc.DepthClipEnable = true;
	noCullDesc.ScissorEnable = false;
	noCullDesc.MultisampleEnable = true;
	noCullDesc.AntialiasedLineEnable = false;
	hr = device->CreateRasterizerState(&noCullDesc, &rasterrizer);
	if (FAILED(hr))
	{
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
	hr = device->CreateSamplerState(&sampDesc, &samplerState);
	if (FAILED(hr)) {
		assert(0);
	}
	//UNIT06
	D3D11_DEPTH_STENCIL_DESC depthsteDesc;
	ZeroMemory(&depthsteDesc, sizeof(depthsteDesc));
	depthsteDesc.DepthEnable = TRUE;
	depthsteDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthsteDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthsteDesc.StencilEnable = FALSE;
	depthsteDesc.StencilReadMask = 0xFF;
	depthsteDesc.StencilWriteMask = 0xFF;
	depthsteDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthsteDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthsteDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthsteDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthsteDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthsteDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthsteDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthsteDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&depthsteDesc, &depthStencil);
	if (FAILED(hr)) {
		assert(0);
	}
	//if (file_name == L"")
	//{
	//	//ダミーテクスチャ作成
	//	static unsigned char image[]{ 255,255,255 };
	//	D3D11_TEXTURE2D_DESC desc;
	//	D3D11_SUBRESOURCE_DATA initialData;
	//	ID3D11Texture2D* texture = NULL;

	//	//ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	//	desc.Width = 1;
	//	desc.Height = 1;
	//	desc.MipLevels = 1;
	//	desc.ArraySize = 1;
	//	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	//	desc.SampleDesc.Count = 1;
	//	desc.SampleDesc.Quality = 0;
	//	desc.Usage = D3D11_USAGE_DEFAULT;
	//	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//	desc.CPUAccessFlags = 0;
	//	desc.MiscFlags = 0;

	//	initialData.pSysMem = image;
	//	initialData.SysMemPitch = 1;
	//	initialData.SysMemSlicePitch = 0;

	//	hr = device->CreateTexture2D(&desc, &initialData, &texture);
	//	if (FAILED(hr)) {
	//		assert(0);
	//	}
	//	ID3D11ShaderResourceView* textureRview = NULL;
	//	//ID3DX11EffectShaderResourceVariable* textureRVar = NULL;

	//	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	//	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	//	SRVDesc.Format = desc.Format;
	//	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//	SRVDesc.Texture2D.MostDetailedMip = 0;
	//	SRVDesc.Texture2D.MipLevels = 1;

	//	ID3D11Resource*resource = nullptr;
	//	const D3D11_SHADER_RESOURCE_VIEW_DESC shader_resoirce_view_desc;
	//	hr = device->CreateShaderResourceView(texture, &SRVDesc, &srview);
	//	if (FAILED(hr)) {
	//		assert(0);
	//	}
	//}
	//else
	//{
	hr =load_texture_from_file(device, file_name, &srview, &texture2d);
	if (FAILED(hr)) {
		assert(0);
	}


}

//Board::~Board()
//{
//	if (vertexshader != nullptr)
//	{
//		vertexshader->Release();
//		vertexshader = nullptr;
//	}
//	if (pixe != nullptr)
//	{
//		pixe->Release();
//		pixe = nullptr;
//	}
//	if (buffer != nullptr)
//	{
//		buffer->Release();
//		buffer = nullptr;
//	}
//	if (rasterrizer != nullptr)
//	{
//		rasterrizer->Release();
//		rasterrizer = nullptr;
//	}
//	if (srview != nullptr)
//	{
//		srview->Release();
//		srview = nullptr;
//	}
//	if (samplerstate != nullptr)
//	{
//		samplerstate->Release();
//		samplerstate = nullptr;
//	}
//	if (depthstencil != nullptr)
//	{
//		depthstencil->Release();
//		depthstencil = nullptr;
//	}
//	if (input != nullptr)
//	{
//		input->Release();
//		input = nullptr;
//	}
//	if (constant_buffer != nullptr)
//	{
//		constant_buffer->Release();
//		constant_buffer = nullptr;
//	}
//
//}

void Board::render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT3& position, const float& scale, const DirectX::XMFLOAT3& angle,
	const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& color)
{
	UINT stride = sizeof(vertex);
	UINT offset = 0;

	immediate_context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);

	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	immediate_context->IASetInputLayout(input);

	immediate_context->RSSetState(rasterrizer);

	immediate_context->VSSetShader(vertexShader, NULL, 0);

	immediate_context->PSSetShader(pixelShader, NULL, 0);
	immediate_context->PSSetShaderResources(0, 1, &srview);

	immediate_context->PSSetSamplers(0, 1, &samplerState);
	immediate_context->OMSetDepthStencilState(depthStencil, 0);

	{
		//scaleとpositionからワールド行列を作成
		DirectX::XMMATRIX S, R, T, W, WVP;
		{
			S = DirectX::XMMatrixScaling(scale, scale, scale);
			//R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
			W = S /** R */* T;
		}
		//	Matrix型へ変換
		DirectX::XMMATRIX V, P;
		V = DirectX::XMLoadFloat4x4(&view);
		P = DirectX::XMLoadFloat4x4(&projection);
		DirectX::XMFLOAT4X4	view2 = view;
		DirectX::XMMATRIX		inv_view2;
		//	位置情報だけを削除
		view2._41 = 0.0f; view2._42 = 0.0f;
		view2._43 = 0.0f; view2._44 = 1.0f;
		inv_view2 = DirectX::XMLoadFloat4x4(&view2);
		//	view行列の逆行列作成
		inv_view2 = DirectX::XMMatrixInverse(nullptr, inv_view2);
		WVP = inv_view2 * W * V * P;
		DirectX::XMFLOAT4X4 wvp;
		DirectX::XMStoreFloat4x4(&wvp, WVP);
		//
		cbuffer cb;
		cb.worldViewProjection = wvp;
		cb.materialColor = color;
		immediate_context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
		immediate_context->VSSetConstantBuffers(0, 1, &constantBuffer);
		immediate_context->PSSetConstantBuffers(0, 1, &constantBuffer);

	}

	immediate_context->Draw(4, 0);

}


