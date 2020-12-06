
#include <stdio.h> 
#include"GeometricPrimitive.h"

#include "misc.h"
#include<vector>

void GeometricPrimitive::createbuffers(ID3D11Device* device, vertex* vertex_, int vertex_num, u_int* index_, int index_num)
{
	HRESULT hr = S_OK;
	num = index_num;
	// 頂点バッファの生成
	{
		// 頂点バッファを作成するための設定オプション
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(vertex) * vertex_num;	// バッファ（データを格納する入れ物）のサイズ
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファとしてバッファを作成する。
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		// 頂点バッファに頂点データを入れるための設定
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pSysMem = vertex_;	// ここに格納したい頂点データのアドレスを渡すことでCreateBuffer()時にデータを入れることができる。
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
											   // 頂点バッファオブジェクトの生成
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	// インテックスバッファの生成
	{
		// インテックスバッファを作成するための設定オプション
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(u_int) * index_num;	// バッファ（データを格納する入れ物）のサイズ
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;//またはD3D11_USAGE_IMMUTABLE
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;	// インテックスバッファとしてバッファを作成する。
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		// インテックスバッファにインテックスデータを入れるための設定
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pSysMem = index_;	// ここに格納したいインテックスデータのアドレスを渡すことでCreateBuffer()時にデータを入れることができる。
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
											   // インテックスバッファオブジェクトの生成
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, &index_buffer);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	// 定数バッファの生成
	{
		// 定数バッファを作成するための設定オプション
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(cbuffer);	// バッファ（データを格納する入れ物）のサイズ
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 定数バッファとしてバッファを作成する。
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, &constant_buffer);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 頂点シェーダー＆頂点入力レイアウトの生成
	{
		// 頂点シェーダーファイルを開く(sprite_vs.hlsl をコンパイルしてできたファイル)
		FILE* fp = nullptr;
		fopen_s(&fp, "Data/shaders/cso/geometric_primitive_vs.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// 頂点シェーダーファイルのサイズを求める
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// メモリ上に頂点シェーダーデータを格納する領域を用意する
		unsigned char* cso_data = new unsigned char[cso_sz];
		fread(cso_data, cso_sz, 1, fp);	// 用意した領域にデータを読み込む
		fclose(fp);	// ファイルを閉じる

					// 頂点シェーダーデータを基に頂点シェーダーオブジェクトを生成する
		HRESULT hr = device->CreateVertexShader(
			cso_data,		// 頂点シェーダーデータのポインタ
			cso_sz,			// 頂点シェーダーデータサイズ
			nullptr,
			&vertex_shader	// 頂点シェーダーオブジェクトのポインタの格納先。
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// GPUに頂点データの内容を教えてあげるための設定
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 入力レイアウトの生成
		hr = device->CreateInputLayout(
			input_element_desc,				// 頂点データの内容
			ARRAYSIZE(input_element_desc),	// 頂点データの要素数
			cso_data,						// 頂点シェーダーデータ（input_element_descの内容と sprite_vs.hlslの内容に不一致がないかチェックするため）
			cso_sz,							// 頂点シェーダーデータサイズ
			&input_layout					// 入力レイアウトオブジェクトのポインタの格納先。
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// 頂点シェーダーデータの後始末
		delete[] cso_data;
	}

	// ピクセルシェーダーの生成
	{
		// ピクセルシェーダーファイルを開く(sprite_ps.hlsl をコンパイルしてできたファイル)
		FILE* fp = nullptr;
		fopen_s(&fp, "Data/shaders/cso/geometric_primitive_ps.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// ピクセルシェーダーファイルのサイズを求める
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// メモリ上にピクセルシェーダーデータを格納する領域を用意する
		unsigned char* cso_data = new unsigned char[cso_sz];
		fread(cso_data, cso_sz, 1, fp);	// 用意した領域にデータを読み込む
		fclose(fp);	// ファイルを閉じる

					// ピクセルシェーダーの生成
		HRESULT hr = device->CreatePixelShader(
			cso_data,		// ピクセルシェーダーデータのポインタ
			cso_sz,			// ピクセルシェーダーデータサイズ
			nullptr,
			&pixel_shader	// ピクセルシェーダーオブジェクトのポインタの格納先
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// ピクセルシェーダーデータの後始末
		delete[] cso_data;
	}

	// ラスタライザステートの生成
	{
		// ラスタライザステートを作成するための設定オプション
		D3D11_RASTERIZER_DESC rasterizer_desc = {}; //https://msdn.microsoft.com/en-us/library/windows/desktop/ff476198(v=vs.85).aspx
		rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE：両面描画, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizer_desc.FrontCounterClockwise = FALSE;//これがFALSEなら時計回り、TRUEなら反時計回り
		rasterizer_desc.DepthBias = 0; //https://msdn.microsoft.com/en-us/library/windows/desktop/cc308048(v=vs.85).aspx
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = FALSE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;

		// ラスタライザステートの生成
		hr = device->CreateRasterizerState(
			&rasterizer_desc,
			&rasterizer_state
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

}


void GeometricPrimitive::render(ID3D11DeviceContext* immediate_context,
	const DirectX::XMFLOAT4& color,
	const DirectX::XMFLOAT4& light_direction,
	const DirectX::XMFLOAT4X4& world_view_projection,
	const DirectX::XMFLOAT4X4& world) const
{
	//定数バッファの内容を更新
	{
		cbuffer data;
		data.world_view_projection = world_view_projection;
		data.material_color = color;
		data.light_direction = light_direction;
		data.world = world;
		immediate_context->UpdateSubresource(constant_buffer, 0, nullptr, &data, 0, 0);//定数バッファの内容を変えるときこれを使う
		immediate_context->VSSetConstantBuffers(0, 1, &constant_buffer);//頂点シェーダに定数バッファの情報を送る
	}
	// 使用する頂点バッファやシェーダーなどをGPUに教えてやる。
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	immediate_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);//DXGI_FORMAT_R32_UINT=unsigned intの32ビット
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);	// ポリゴンの描き方の指定
	immediate_context->IASetInputLayout(input_layout);

	immediate_context->RSSetState(rasterizer_state);

	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);

	// ↑で設定したリソースを利用してポリゴンを描画する。
	//immediate_context->Draw(4, 0);
	immediate_context->DrawIndexed(num, 0, 0);

	immediate_context->IASetInputLayout(0);
	immediate_context->VSSetShader(null_vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(null_pixel_shader, nullptr, 0);
	immediate_context->VSSetConstantBuffers(0, 1, &null_constant_buffer);//頂点シェーダに定数バッファの情報を送る
}

GeometricCube::GeometricCube(ID3D11Device* device) :GeometricPrimitive(device)
{
	// 頂点データの定義
	// 0           1
	// +-----------+
	// |           |
	// |           |
	// +-----------+
	// 2           3
	vertex vertices[] = {
		//手前の面(赤)
		{ DirectX::XMFLOAT3(-0.5, +0.5, -0.5), DirectX::XMFLOAT3(0, 0, -1) },//0
		{ DirectX::XMFLOAT3(+0.5, +0.5, -0.5), DirectX::XMFLOAT3(0, 0, -1) },//1
		{ DirectX::XMFLOAT3(-0.5, -0.5, -0.5), DirectX::XMFLOAT3(0, 0, -1) },//2
		{ DirectX::XMFLOAT3(+0.5, -0.5, -0.5), DirectX::XMFLOAT3(0, 0, -1) },//3
																			 //奥の面(緑)											   3
		{ DirectX::XMFLOAT3(-0.5, +0.5, +0.5), DirectX::XMFLOAT3(0, 0, 1) },//4
		{ DirectX::XMFLOAT3(+0.5, +0.5, +0.5), DirectX::XMFLOAT3(0, 0, 1) },//5
		{ DirectX::XMFLOAT3(-0.5, -0.5, +0.5), DirectX::XMFLOAT3(0, 0, 1) },//6
		{ DirectX::XMFLOAT3(+0.5, -0.5, +0.5), DirectX::XMFLOAT3(0, 0, 1) },//7
																			//右の面(青)											   3
		{ DirectX::XMFLOAT3(+0.5, +0.5, -0.5), DirectX::XMFLOAT3(1, 0, 0) },
		{ DirectX::XMFLOAT3(+0.5, +0.5, +0.5), DirectX::XMFLOAT3(1, 0, 0) },
		{ DirectX::XMFLOAT3(+0.5, -0.5, -0.5), DirectX::XMFLOAT3(1, 0, 0) },
		{ DirectX::XMFLOAT3(+0.5, -0.5, +0.5), DirectX::XMFLOAT3(1, 0, 0) },
		//左の面(紫)											   3
		{ DirectX::XMFLOAT3(-0.5, +0.5, -0.5), DirectX::XMFLOAT3(-1, 0, 0) },
		{ DirectX::XMFLOAT3(-0.5, +0.5, +0.5), DirectX::XMFLOAT3(-1, 0, 0) },
		{ DirectX::XMFLOAT3(-0.5, -0.5, -0.5), DirectX::XMFLOAT3(-1, 0, 0) },
		{ DirectX::XMFLOAT3(-0.5, -0.5, +0.5), DirectX::XMFLOAT3(-1, 0, 0) },
		//上の面(水色)											  3
		{ DirectX::XMFLOAT3(-0.5, +0.5, +0.5), DirectX::XMFLOAT3(0, 1, 0) },
		{ DirectX::XMFLOAT3(+0.5, +0.5, +0.5), DirectX::XMFLOAT3(0, 1, 0) },
		{ DirectX::XMFLOAT3(-0.5, +0.5, -0.5), DirectX::XMFLOAT3(0, 1, 0) },
		{ DirectX::XMFLOAT3(+0.5, +0.5, -0.5), DirectX::XMFLOAT3(0, 1, 0) },
		//下の面(白)											   3
		{ DirectX::XMFLOAT3(-0.5, -0.5, +0.5), DirectX::XMFLOAT3(0, -1, 0) },
		{ DirectX::XMFLOAT3(+0.5, -0.5, +0.5), DirectX::XMFLOAT3(0, -1, 0) },
		{ DirectX::XMFLOAT3(-0.5, -0.5, -0.5), DirectX::XMFLOAT3(0, -1, 0) },
		{ DirectX::XMFLOAT3(+0.5, -0.5, -0.5), DirectX::XMFLOAT3(0, -1, 0) },
	};

	//インデックスデータ(時計回りに書いたら表面が見えて反対にすると裏面が見える)
	unsigned int indices[] = {
		//手前の面(時計回り)
		0,1,2,
		2,1,3,
		//奥の面(反時計回り)
		5,4,7,
		7,4,6,
		//右の面(時計回り)
		8,9,10,
		10,9,11,
		//左の面(反時計回り)
		12,14,13,
		13,14,15,
		//上の面(時計回り)
		16,17,18,
		18,17,19,
		//下の面(反時計回り)
		20,22,21,
		21,22,23,
	};
	createbuffers(device, vertices, 24, indices, 36);
}

GeometricSphere::GeometricSphere(ID3D11Device* device, u_int slices, u_int stacks) :GeometricPrimitive(device)
{
	std::vector<vertex> vertices;
	std::vector<u_int> indices;


	float r = 0.5f;		//	半径 0.5f = 直径 1.0f

						//
						// Compute the vertices stating at the top pole and moving down the stacks.
						//

						// Poles: note that there will be texture coordinate distortion as there is
						// not a unique point on the texture map to assign to the pole when mapping
						// a rectangular texture onto a sphere.
	vertex top_vertex;
	top_vertex.position = DirectX::XMFLOAT3(0.0f, +r, 0.0f);
	top_vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);

	vertex bottom_vertex;
	bottom_vertex.position = DirectX::XMFLOAT3(0.0f, -r, 0.0f);
	bottom_vertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

	vertices.push_back(top_vertex);

	float phi_step = DirectX::XM_PI / stacks;
	float theta_step = 2.0f * DirectX::XM_PI / slices;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (u_int i = 1; i <= stacks - 1; ++i)
	{
		float phi = i * phi_step;
		float rs_phi = r * sinf(phi), rc_phi = r * cosf(phi);

		// Vertices of ring.
		for (u_int j = 0; j <= slices; ++j)
		{
			float theta = j * theta_step;

			vertex v;

			// spherical to cartesian
			v.position.x = rs_phi * cosf(theta);
			v.position.y = rc_phi;
			v.position.z = rs_phi * sinf(theta);

			DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.position);
			DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

			vertices.push_back(v);
		}
	}

	vertices.push_back(bottom_vertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//
	for (UINT i = 1; i <= slices; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	u_int base_index = 1;
	u_int ring_vertex_count = slices + 1;
	for (u_int i = 0; i < stacks - 2; ++i)
	{
		u_int i_rvc = i * ring_vertex_count;
		u_int i1_rvc = (i + 1) * ring_vertex_count;

		for (u_int j = 0; j < slices; ++j)
		{
			indices.push_back(base_index + i_rvc + j);
			indices.push_back(base_index + i_rvc + j + 1);
			indices.push_back(base_index + i1_rvc + j);

			indices.push_back(base_index + i1_rvc + j);
			indices.push_back(base_index + i_rvc + j + 1);
			indices.push_back(base_index + i1_rvc + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	u_int south_pole_index = (u_int)vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	base_index = south_pole_index - ring_vertex_count;

	for (u_int i = 0; i < slices; ++i)
	{
		indices.push_back(south_pole_index);
		indices.push_back(base_index + i);
		indices.push_back(base_index + i + 1);
	}
	createbuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

}

GeometricCylinder::GeometricCylinder(ID3D11Device* device, u_int slices) : GeometricPrimitive(device)
{
	std::vector<vertex> vertices;
	std::vector<u_int> indices;

	float d = 2.0f * DirectX::XM_PI / slices;
	float r = 0.5f;

	vertex vertex;
	u_int base_index = 0;

	// top cap centre
	vertex.position = DirectX::XMFLOAT3(0.0f, +0.5f, 0.0f);
	vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);
	vertices.push_back(vertex);
	// top cap ring
	for (u_int i = 0; i < slices; ++i)
	{
		float x = r * cosf(i * d);
		float z = r * sinf(i * d);
		vertex.position = DirectX::XMFLOAT3(x, +0.5f, z);
		vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);
		vertices.push_back(vertex);
	}
	base_index = 0;
	for (u_int i = 0; i < slices - 1; ++i)
	{
		indices.push_back(base_index + 0);
		indices.push_back(base_index + i + 2);
		indices.push_back(base_index + i + 1);
	}
	indices.push_back(base_index + 0);
	indices.push_back(base_index + 1);
	indices.push_back(base_index + slices);

	// bottom cap centre
	vertex.position = DirectX::XMFLOAT3(0.0f, -0.5f, 0.0f);
	vertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertices.push_back(vertex);
	// bottom cap ring
	for (u_int i = 0; i < slices; ++i)
	{
		float x = r * cosf(i * d);
		float z = r * sinf(i * d);
		vertex.position = DirectX::XMFLOAT3(x, -0.5f, z);
		vertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
		vertices.push_back(vertex);
	}
	base_index = slices + 1;
	for (u_int i = 0; i < slices - 1; ++i)
	{
		indices.push_back(base_index + 0);
		indices.push_back(base_index + i + 1);
		indices.push_back(base_index + i + 2);
	}
	indices.push_back(base_index + 0);
	indices.push_back(base_index + (slices - 1) + 1);
	indices.push_back(base_index + (0) + 1);

	// side rectangle
	for (u_int i = 0; i < slices; ++i)
	{
		float x = r * cosf(i * d);
		float z = r * sinf(i * d);

		vertex.position = DirectX::XMFLOAT3(x, +0.5f, z);
		vertex.normal = DirectX::XMFLOAT3(x, 0.0f, z);
		vertices.push_back(vertex);

		vertex.position = DirectX::XMFLOAT3(x, -0.5f, z);
		vertex.normal = DirectX::XMFLOAT3(x, 0.0f, z);
		vertices.push_back(vertex);
	}
	base_index = slices * 2 + 2;
	for (u_int i = 0; i < slices - 1; ++i)
	{
		indices.push_back(base_index + i * 2 + 0);
		indices.push_back(base_index + i * 2 + 2);
		indices.push_back(base_index + i * 2 + 1);

		indices.push_back(base_index + i * 2 + 1);
		indices.push_back(base_index + i * 2 + 2);
		indices.push_back(base_index + i * 2 + 3);
	}
	indices.push_back(base_index + (slices - 1) * 2 + 0);
	indices.push_back(base_index + (0) * 2 + 0);
	indices.push_back(base_index + (slices - 1) * 2 + 1);

	indices.push_back(base_index + (slices - 1) * 2 + 1);
	indices.push_back(base_index + (0) * 2 + 0);
	indices.push_back(base_index + (0) * 2 + 1);

	createbuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

}

GeometricCapsule::GeometricCapsule(ID3D11Device* device) : GeometricPrimitive(device)
{

}
