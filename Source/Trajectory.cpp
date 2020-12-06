#include "Trajectory.h"
#include "ShaderEX.h"
#include "misc.h"
#include "Math.h"
#include "Texture.h"
Trajectory::Trajectory(ID3D11Device* device, const wchar_t* file_name)
{
	HRESULT hr = S_OK;
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	//頂点シェーダーの生成
	create_vs_from_cso(device, "Data/shaders/cso/Trajectory_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	//ピクセルシェーダの生成
	create_ps_from_cso(device, "Data/shaders/cso/Trajectory_ps.cso", pixelShader.GetAddressOf());

	//定数バッファ
	{
		// 定数バッファを作成するための設定オプション
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(cbuffer);	// バッファ（データを格納する入れ物）のサイズ
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 定数バッファとしてバッファを作成する。
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, constantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	hr = load_texture_from_file(device, file_name, &srview, &texture2d);
	if (FAILED(hr)) {
		assert(0);
	}
	count = 0;
}
void Trajectory::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4 viewProjection)
{
	for (int i = 0; i < FREAMECOUNT; i++)
	{
		if (trajectoryStart[i].isFlag)
			count++;
	}
	if (count <= 1)return;
	if (count >= FREAMECOUNT)
	{
		count = FREAMECOUNT - 1;
	}
#if 0
	{
		float amout = 1.0f / FREAMECOUNT - 1;
		float v = 0;
		int c = 0;
		vertices.clear();
		for (int i = 0; i < FREAMECOUNT; i++)
		{
			vertex ver;

			ver.position = trajectoryStart[i].position;
			ver.texcoord = DirectX::XMFLOAT2(1.0f, v);
			ver.color = DirectX::XMFLOAT4(color);
			vertices.push_back(ver);

			ver.position = trajectoryEnd[i].position;
			ver.texcoord = DirectX::XMFLOAT2(0.0f, v);
			ver.color = DirectX::XMFLOAT4(color);
			vertices.push_back(ver);
			v += amout;
		}
	}
#else 
	catmullRomEndPosition.clear();
	catmullRomStartPosition.clear();
	for (int i = 0; i < FREAMECOUNT; i++) //補完
	{
		//先端
		DirectX::XMFLOAT3 posS0 = trajectoryStart[i - 1].position;
		DirectX::XMFLOAT3 posS1 = trajectoryStart[i].position;
		DirectX::XMFLOAT3 posS2 = trajectoryStart[i + 1].position;
		DirectX::XMFLOAT3 posS3 = trajectoryStart[i + 2].position;

		DirectX::XMFLOAT3 posE0 = trajectoryEnd[i - 1].position;
		DirectX::XMFLOAT3 posE1 = trajectoryEnd[i].position;
		DirectX::XMFLOAT3 posE2 = trajectoryEnd[i + 1].position;
		DirectX::XMFLOAT3 posE3 = trajectoryEnd[i + 2].position;
		if (i == 0)
		{
			//posS0 = Vec3Multiply(Vec3Add(posS1, posS2), 0.5);
			posS0 = Vec3Add(posS1, Vec3Multiply(posS2, 0.5));

			//posE0 = Vec3Multiply(Vec3Add(posE1, posE2), 0.5);
			posE0 = Vec3Add(posE1, Vec3Multiply(posE2, 0.5));
		}
		if (i + 2 >= FREAMECOUNT)
		{
			//posS3 = Vec3Multiply(Vec3Add(posS0, posS2), 0.5);
			posS3 = Vec3Add(posS0, Vec3Multiply(posS2, 0.5f));

			//posE3 = Vec3Multiply(Vec3Add(posE0, posE2), 0.5);
			posE3 = Vec3Add(posE0, Vec3Multiply(posE2, 0.5f));
		}

		DirectX::XMVECTOR posVS0 = DirectX::XMLoadFloat3(&posS0);
		DirectX::XMVECTOR posVS1 = DirectX::XMLoadFloat3(&posS1);
		DirectX::XMVECTOR posVS2 = DirectX::XMLoadFloat3(&posS2);
		DirectX::XMVECTOR posVS3 = DirectX::XMLoadFloat3(&posS3);

		DirectX::XMVECTOR posVE0 = DirectX::XMLoadFloat3(&posE0);
		DirectX::XMVECTOR posVE1 = DirectX::XMLoadFloat3(&posE1);
		DirectX::XMVECTOR posVE2 = DirectX::XMLoadFloat3(&posE2);
		DirectX::XMVECTOR posVE3 = DirectX::XMLoadFloat3(&posE3);

		DirectX::XMVECTOR catmullRomVPosition;
		DirectX::XMFLOAT3 catmullRomPosition;
		for (float tt = 0.25f; tt < 1.00f; tt += 0.25)
		{
			catmullRomVPosition = DirectX::XMVectorCatmullRom(posVS0, posVS1, posVS2, posVS3, tt);
			DirectX::XMStoreFloat3(&catmullRomPosition, catmullRomVPosition);
			catmullRomStartPosition.push_back(catmullRomPosition);

			catmullRomVPosition = DirectX::XMVectorCatmullRom(posVE0, posVE1, posVE2, posVE3, tt);
			DirectX::XMStoreFloat3(&catmullRomPosition, catmullRomVPosition);
			catmullRomEndPosition.push_back(catmullRomPosition);
		}

		if (i + 2 >= FREAMECOUNT)break;
	}

	float   size = static_cast<float>(catmullRomEndPosition.size());
	float amout = 1.0f / size;
	w = 0.0f;
	float v = 0;
	int c = 0;
	vertices.clear();
	for (float i = 0; i < size; i++)
	{
		vertex ver;
		//徐々に薄く
		w = 1 - i / size;

		ver.position = catmullRomStartPosition[i];
		ver.texcoord = DirectX::XMFLOAT2(1.0f, v);
		//ver.color = DirectX::XMFLOAT4(0.5f, 0.4f, 0.3f, 1.0f);
		ver.color = DirectX::XMFLOAT4(color.x, color.y, color.z, w);
		vertices.push_back(ver);
		//vertices[i].color = DirectX::XMFLOAT4(c.x, c.y, c.z, 1.f);

		ver.position = catmullRomEndPosition[i];
		ver.texcoord = DirectX::XMFLOAT2(0.0f, v);
		//	ver.color = DirectX::XMFLOAT4(0.5f, 0.4f, 0.3f, 1.0f);
		ver.color = DirectX::XMFLOAT4(color.x, color.y, color.z, w);
		vertices.push_back(ver);
		//vertices[i + 1].color = DirectX::XMFLOAT4(c.x, c.y, c.z, 1.f);
		v += amout;
	}
#endif
	HRESULT hr = S_OK;
	// 頂点バッファの生成
	{
		D3D11_BUFFER_DESC structure = {};
		ZeroMemory(&structure, sizeof(structure));
		structure.Usage = D3D11_USAGE_DYNAMIC;
		structure.ByteWidth = /*sizeof(vertex) * VERTEXNUM; */vertices.size() * sizeof(vertex);
		structure.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		structure.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		D3D11_SUBRESOURCE_DATA structure_data;
		ZeroMemory(&structure_data, sizeof(structure_data));
		structure_data.pSysMem = vertices.data();


		Microsoft::WRL::ComPtr < ID3D11Device>device;
		immediate_context->GetDevice(device.GetAddressOf());
		hr = device->CreateBuffer(&structure, &structure_data, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(inputLayout.Get());
	immediate_context->VSSetShader(vertexShader.Get(), NULL, 0);
	immediate_context->PSSetShader(pixelShader.Get(), NULL, 0);
	immediate_context->PSSetShaderResources(0, 1, srview.GetAddressOf());
	cbuffer cb;
	cb.view_projection = viewProjection;
	immediate_context->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	immediate_context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	immediate_context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	immediate_context->Draw(vertices.size(), 0);
	//immediate_context->Draw(VERTEXNUM, 0);
}

void Trajectory::SwapPosition()
{
	for (int i = FREAMECOUNT - 1; i > 0; i--)
	{

		trajectoryStart[i] = trajectoryStart[i - 1];
		trajectoryEnd[i] = trajectoryEnd[i - 1];
	}
	trajectoryStart[0].position = startPosition;
	trajectoryStart[0].isFlag = true;
	trajectoryEnd[0].position = endPosition;
	trajectoryEnd[0].isFlag = true;

}
