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
//	// ���̓��C�A�E�g
//	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	//���_�V�F�[�_�[�̐���
//	create_vs_from_cso(device, "Data/shaders/cso/BoardPoly_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
//	//�s�N�Z���V�F�[�_�̐���
//	create_ps_from_cso(device, "Data/shaders/cso/BoardPoly_ps.cso", pixelShader.GetAddressOf());
//
//	// ���_�o�b�t�@�̐���
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
//	//�萔�o�b�t�@
//	{
//		// �萔�o�b�t�@���쐬���邽�߂̐ݒ�I�v�V����
//		D3D11_BUFFER_DESC buffer_desc = {};
//		buffer_desc.ByteWidth = sizeof(cbuffer);	// �o�b�t�@�i�f�[�^���i�[������ꕨ�j�̃T�C�Y
//		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// �萔�o�b�t�@�Ƃ��ăo�b�t�@���쐬����B
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
//	//	Matrix�^�֕ϊ�
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
