//#include "AttackCircle.h"
//#include "ShaderEX.h"
//#include"misc.h"
//#include <functional>
//AttackCircle* AttackCircle::attackCircle = nullptr;
//
//
//void AttackCircle::createBuffers(ID3D11Device* device, vertex* vertices)
//{
//	HRESULT hr = S_OK;
//
//	// 入力レイアウト
//	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	//頂点シェーダーの生成
//	create_vs_from_cso(device, "Data/shaders/cso/BoardPoly_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
//	//ピクセルシェーダの生成
//	create_ps_from_cso(device, "Data/shaders/cso/BoardPoly_ps.cso", pixelShader.GetAddressOf());
//
//	// 頂点バッファの生成
//	{
//		D3D11_BUFFER_DESC structure = {};
//		ZeroMemory(&structure, sizeof(structure));
//		structure.Usage = D3D11_USAGE_DYNAMIC;
//		structure.ByteWidth = sizeof(vertex) * 4;
//		structure.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		structure.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//		D3D11_SUBRESOURCE_DATA structure_data;
//		ZeroMemory(&structure_data, sizeof(structure_data));
//		structure_data.pSysMem = vertices;
//		hr = device->CreateBuffer(&structure, &structure_data, vertexBuffer.GetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
//	}
//	//定数バッファ
//	{
//		// 定数バッファを作成するための設定オプション
//		D3D11_BUFFER_DESC buffer_desc = {};
//		buffer_desc.ByteWidth = sizeof(cbuffer);	// バッファ（データを格納する入れ物）のサイズ
//		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 定数バッファとしてバッファを作成する。
//		buffer_desc.CPUAccessFlags = 0;
//		buffer_desc.MiscFlags = 0;
//		buffer_desc.StructureByteStride = 0;
//		hr = device->CreateBuffer(&buffer_desc, nullptr, constantBuffer.GetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
//	}
//}
//
//void AttackCircle::Update()
//{
//	DirectX::XMMATRIX S, R, T, WVP;
//	{
//		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
//		R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
//		T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
//		world = S * R * T;
//	}
//}
//
//void AttackCircle::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4 viewProjection, const DirectX::XMFLOAT4& color)
//{
//	UINT stride = sizeof(vertex);
//	UINT offset = 0;
//	immediate_context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
//	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//	immediate_context->IASetInputLayout(inputLayout.Get());
//	immediate_context->VSSetShader(vertexShader.Get(), NULL, 0);
//	immediate_context->PSSetShader(pixelShader.Get(), NULL, 0);
//
//	//	Matrix型へ変換
//	DirectX::XMMATRIX VP;
//	DirectX::XMMATRIX WVP;
//	VP = DirectX::XMLoadFloat4x4(&viewProjection);
//	WVP = world * VP;
//	DirectX::XMFLOAT4X4 wvp;
//	DirectX::XMStoreFloat4x4(&wvp, WVP);
//	cbuffer cb;
//
//	cb.world_view_projection = wvp;
//	cb.material_color = color;
//	immediate_context->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
//	immediate_context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
//	immediate_context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
//
//	immediate_context->Draw(4, 0);
//}
