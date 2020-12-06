
#include "TexMesh.h"

TexCube::TexCube(ID3D11Device* device, const wchar_t* filename) : TexMesh(device, filename)
{
	//頂点を定義
	Vertex vertices[24] =
	{
		VECTOR3(0.5f,	0.5f,  -0.5f),	VECTOR3(0.0f, 0.0f, -1.0f), VECTOR2(1,0),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,  0.5f, -0.5f),	VECTOR3(0.0f, 0.0f, -1.0f), VECTOR2(0,0),VECTOR4(1,1,1,1),
		VECTOR3(0.5f,  -0.5f, -0.5f),	VECTOR3(0.0f, 0.0f, -1.0f), VECTOR2(1,1),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f, -0.5f, -0.5f),	VECTOR3(0.0f, 0.0f, -1.0f), VECTOR2(0,1),VECTOR4(1,1,1,1),

		VECTOR3(-0.5f,	0.5f, 0.5f),	VECTOR3(0.0f, 0.0f, 1.0f),  VECTOR2(1,0),VECTOR4(1,1,1,1),
		VECTOR3(0.5f,  0.5f, 0.5f),	VECTOR3(0.0f, 0.0f, 1.0f),  VECTOR2(0,0),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,-0.5f, 0.5f),	VECTOR3(0.0f, 0.0f, 1.0f),  VECTOR2(1,1),VECTOR4(1,1,1,1),
		VECTOR3(0.5f, -0.5f, 0.5f),	VECTOR3(0.0f, 0.0f, 1.0f),  VECTOR2(0,1),VECTOR4(1,1,1,1),

		VECTOR3(0.5f,	0.5f, 0.5f),	VECTOR3(1.0f, 0.0f, 0.0f),  VECTOR2(1,0),VECTOR4(1,1,1,1),
		VECTOR3(0.5f,  0.5f, -0.5f),	VECTOR3(1.0f, 0.0f, 0.0f),  VECTOR2(0,0),VECTOR4(1,1,1,1),
		VECTOR3(0.5f, -0.5f, 0.5f),	VECTOR3(1.0f, 0.0f, 0.0f),  VECTOR2(1,1),VECTOR4(1,1,1,1),
		VECTOR3(0.5f, -0.5f, -0.5f),	VECTOR3(1.0f, 0.0f, 0.0f),  VECTOR2(0,1),VECTOR4(1,1,1,1),

		VECTOR3(-0.5f,	0.5f, -0.5f),	VECTOR3(-1.0f, 0.0f, 0.0f), VECTOR2(1,0),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,  0.5f, 0.5f),	VECTOR3(-1.0f, 0.0f, 0.0f), VECTOR2(0,0),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f, -0.5f, -0.5f),	VECTOR3(-1.0f, 0.0f, 0.0f), VECTOR2(1,1),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f, -0.5f, 0.5f),	VECTOR3(-1.0f, 0.0f, 0.0f), VECTOR2(0,1),VECTOR4(1,1,1,1),

		VECTOR3(0.5f,	 0.5f,  0.5f),	VECTOR3(0.0f, 1.0f, 0.0f),  VECTOR2(1,0),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,  0.5f,  0.5f),	VECTOR3(0.0f, 1.0f, 0.0f),  VECTOR2(0,0),VECTOR4(1,1,1,1),
		VECTOR3(0.5f,  0.5f, -0.5f),	VECTOR3(0.0f, 1.0f, 0.0f),  VECTOR2(1,1),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,  0.5f, -0.5f),	VECTOR3(0.0f, 1.0f, 0.0f),  VECTOR2(0,1),VECTOR4(1,1,1,1),

		VECTOR3(0.5f,	 -0.5f, -0.5f),	VECTOR3(0.0f, -1.0f, 0.0f), VECTOR2(1,0),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,  -0.5f, -0.5f),	VECTOR3(0.0f, -1.0f, 0.0f), VECTOR2(0,0),VECTOR4(1,1,1,1),
		VECTOR3(0.5f,  -0.5f, 0.5f),	VECTOR3(0.0f, -1.0f, 0.0f), VECTOR2(1,1),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,  -0.5f, 0.5f),	VECTOR3(0.0f, -1.0f, 0.0f), VECTOR2(0,1),VECTOR4(1,1,1,1),

	};
	int numV = sizeof(vertices) / sizeof(vertices[0]);

	Face face;
	int materialIndex = 0;
	for (int i = 0; i < numV; i++)
	{
		int vertexNum = i % 3;
		if ((vertexNum == 0) && i != 0)
		{
			face.materialIndex = materialIndex;
			mesh.faces.push_back(face);
			materialIndex++;
		}
		face.position[vertexNum] = vertices[i].position;
	}

	//インデックスを定義
	u_int indices[36];
	for (int face = 0; face < 6; face++) {
		indices[face * 6] = face * 4;
		indices[face * 6 + 1] = face * 4 + 2;
		indices[face * 6 + 2] = face * 4 + 1;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 2;
		indices[face * 6 + 5] = face * 4 + 3;
	}
	int numI = sizeof(indices) / sizeof(indices[0]);

	CreateBuffers(device, vertices, numV, indices, numI);
}

TexCube::TexCube(ID3D11Device* device, std::shared_ptr<Texture>& texture) : TexMesh(device, texture)
{
	//頂点を定義
	Vertex vertices[24] =
	{
		VECTOR3(0.5f,	0.5f,  -0.5f),	VECTOR3(0.0f, 0.0f, -1.0f), VECTOR2(1,0),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,  0.5f, -0.5f),	VECTOR3(0.0f, 0.0f, -1.0f), VECTOR2(0,0),VECTOR4(1,1,1,1),
		VECTOR3(0.5f,  -0.5f, -0.5f),	VECTOR3(0.0f, 0.0f, -1.0f), VECTOR2(1,1),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f, -0.5f, -0.5f),	VECTOR3(0.0f, 0.0f, -1.0f), VECTOR2(0,1),VECTOR4(1,1,1,1),

		VECTOR3(-0.5f,	0.5f, 0.5f),	VECTOR3(0.0f, 0.0f, 1.0f),  VECTOR2(1,0),VECTOR4(1,1,1,1),
		VECTOR3(0.5f,  0.5f, 0.5f),	VECTOR3(0.0f, 0.0f, 1.0f),  VECTOR2(0,0),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,-0.5f, 0.5f),	VECTOR3(0.0f, 0.0f, 1.0f),  VECTOR2(1,1),VECTOR4(1,1,1,1),
		VECTOR3(0.5f, -0.5f, 0.5f),	VECTOR3(0.0f, 0.0f, 1.0f),  VECTOR2(0,1),VECTOR4(1,1,1,1),

		VECTOR3(0.5f,	0.5f, 0.5f),	VECTOR3(1.0f, 0.0f, 0.0f),  VECTOR2(1,0),VECTOR4(1,1,1,1),
		VECTOR3(0.5f,  0.5f, -0.5f),	VECTOR3(1.0f, 0.0f, 0.0f),  VECTOR2(0,0),VECTOR4(1,1,1,1),
		VECTOR3(0.5f, -0.5f, 0.5f),	VECTOR3(1.0f, 0.0f, 0.0f),  VECTOR2(1,1),VECTOR4(1,1,1,1),
		VECTOR3(0.5f, -0.5f, -0.5f),	VECTOR3(1.0f, 0.0f, 0.0f),  VECTOR2(0,1),VECTOR4(1,1,1,1),

		VECTOR3(-0.5f,	0.5f, -0.5f),	VECTOR3(-1.0f, 0.0f, 0.0f), VECTOR2(1,0),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,  0.5f, 0.5f),	VECTOR3(-1.0f, 0.0f, 0.0f), VECTOR2(0,0),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f, -0.5f, -0.5f),	VECTOR3(-1.0f, 0.0f, 0.0f), VECTOR2(1,1),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f, -0.5f, 0.5f),	VECTOR3(-1.0f, 0.0f, 0.0f), VECTOR2(0,1),VECTOR4(1,1,1,1),

		VECTOR3(0.5f,	 0.5f,  0.5f),	VECTOR3(0.0f, 1.0f, 0.0f),  VECTOR2(1,0),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,  0.5f,  0.5f),	VECTOR3(0.0f, 1.0f, 0.0f),  VECTOR2(0,0),VECTOR4(1,1,1,1),
		VECTOR3(0.5f,  0.5f, -0.5f),	VECTOR3(0.0f, 1.0f, 0.0f),  VECTOR2(1,1),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,  0.5f, -0.5f),	VECTOR3(0.0f, 1.0f, 0.0f),  VECTOR2(0,1),VECTOR4(1,1,1,1),

		VECTOR3(0.5f,	 -0.5f, -0.5f),	VECTOR3(0.0f, -1.0f, 0.0f), VECTOR2(1,0),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,  -0.5f, -0.5f),	VECTOR3(0.0f, -1.0f, 0.0f), VECTOR2(0,0),VECTOR4(1,1,1,1),
		VECTOR3(0.5f,  -0.5f, 0.5f),	VECTOR3(0.0f, -1.0f, 0.0f), VECTOR2(1,1),VECTOR4(1,1,1,1),
		VECTOR3(-0.5f,  -0.5f, 0.5f),	VECTOR3(0.0f, -1.0f, 0.0f), VECTOR2(0,1),VECTOR4(1,1,1,1),

	};
	int numV = sizeof(vertices) / sizeof(vertices[0]);
	Face face;
	int materialIndex = 0;
	for (int i = 0; i < numV; i++)
	{
		int vertexNum = i % 3;
		if ((vertexNum == 0) && i != 0)
		{
			face.materialIndex = materialIndex;
			mesh.faces.push_back(face);
			materialIndex++;
		}
		face.position[vertexNum] = vertices[i].position;
	}

	//インデックスを定義
	u_int indices[36];
	for (int face = 0; face < 6; face++) {
		indices[face * 6] = face * 4;
		indices[face * 6 + 1] = face * 4 + 2;
		indices[face * 6 + 2] = face * 4 + 1;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 2;
		indices[face * 6 + 5] = face * 4 + 3;
	}
	int numI = sizeof(indices) / sizeof(indices[0]);

	CreateBuffers(device, vertices, numV, indices, numI);
}

void TexMesh::CreateBuffers(ID3D11Device* device, Vertex* vertices, int numV, u_int* indices, int numI)
{
	HRESULT hr;

	D3D11_BUFFER_DESC desc = {};
	D3D11_SUBRESOURCE_DATA data = {};

	mesh.numV = numV;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(Vertex) * mesh.numV;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ZeroMemory(&data, sizeof(data));
	data.pSysMem = vertices;//変更
	hr = device->CreateBuffer(&desc, &data, mesh.vertexBuffer.GetAddressOf());

	if (FAILED(hr)) assert(!"vertexBuffer");


	mesh.numI = numI;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(u_int) * mesh.numI;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ZeroMemory(&data, sizeof(data));
	data.pSysMem = indices;
	hr = device->CreateBuffer(&desc, &data, mesh.indexBuffer.GetAddressOf());
	if (FAILED(hr)) assert(!"indexBuffer");

}

TexSphere::TexSphere(ID3D11Device* device, const wchar_t* filename, int slices, int stacks) : TexMesh(device, filename)
{
	//頂点数
	int numV = (slices + 1) * (stacks + 1);

	// 頂点定義
	Vertex* vertices = new Vertex[numV];
	for (int y = 0; y < stacks + 1; y++) {
		for (int x = 0; x < slices + 1; x++) {
			int index = y * (slices + 1) + x;
			float h = 0.5f * cosf(y * DirectX::XM_PI / stacks);
			float w = 0.5f * sinf(y * DirectX::XM_PI / stacks);
			float rad_slices = x * DirectX::XM_PI * 2.0f / slices;

			vertices[index].position.x = w * sinf(rad_slices);
			vertices[index].position.y = h;
			vertices[index].position.z = w * cosf(rad_slices);

			vertices[index].normal.x = vertices[index].position.x * 2.0f;
			vertices[index].normal.y = vertices[index].position.y * 2.0f;
			vertices[index].normal.z = vertices[index].position.z * 2.0f;

			vertices[index].texcoord.x = 1.0f - (float)x / slices;
			vertices[index].texcoord.y = (float)y / stacks - 1.0f;
			vertices[index].color = VECTOR4(1, 1, 1, 1);
		}
	}
	Face face;
	int materialIndex = 0;
	for (int i = 0; i < numV; i++)
	{
		int vertexNum = i % 3;
		if ((vertexNum == 0) && i != 0)
		{
			face.materialIndex = materialIndex;
			mesh.faces.push_back(face);
			materialIndex++;
		}
		face.position[vertexNum] = vertices[i].position;
	}

	// インデックス数
	int numI = stacks * slices * 2 * 3;
	// インデックス設定
	u_int* indices = new u_int[numI];
	for (int y = 0; y < stacks; y++) {
		for (int x = 0; x < slices; x++) {
			int face = (y * slices + x);
			int vertices_index = y * (slices + 1) + x;
			indices[face * 6] = vertices_index + 1;
			indices[face * 6 + 1] = vertices_index;
			indices[face * 6 + 2] = vertices_index + (slices + 1);

			indices[face * 6 + 3] = vertices_index + 1;
			indices[face * 6 + 4] = vertices_index + (slices + 1);
			indices[face * 6 + 5] = vertices_index + (slices + 1) + 1;
		}
	}

	CreateBuffers(device, vertices, numV, indices, numI);

	delete[] vertices;
	delete[] indices;
}

TexSphere::TexSphere(ID3D11Device* device, std::shared_ptr<Texture>& texture, int slices, int stacks) : TexMesh(device, texture)
{
	//頂点数
	int numV = (slices + 1) * (stacks + 1);

	// 頂点定義
	Vertex* vertices = new Vertex[numV];
	for (int y = 0; y < stacks + 1; y++) {
		for (int x = 0; x < slices + 1; x++) {
			int index = y * (slices + 1) + x;
			float h = 0.5f * cosf(y * DirectX::XM_PI / stacks);
			float w = 0.5f * sinf(y * DirectX::XM_PI / stacks);
			float rad_slices = x * DirectX::XM_PI * 2.0f / slices;

			vertices[index].position.x = w * sinf(rad_slices);
			vertices[index].position.y = h;
			vertices[index].position.z = w * cosf(rad_slices);

			const XMFLOAT3 p = vertices[index].position;
			vertices[index].normal.x = p.x * 2.0f;
			vertices[index].normal.y = p.y * 2.0f;
			vertices[index].normal.z = p.z * 2.0f;

			vertices[index].texcoord.x = 1.0f - (float)x / slices;
			vertices[index].texcoord.y = (float)y / stacks - 1.0f;
			vertices[index].color = VECTOR4(1, 1, 1, 1);
		}
	}
	Face face;
	int materialIndex = 0;
	for (int i = 0; i < numV; i++)
	{
		int vertexNum = i % 3;
		if ((vertexNum == 0) && i != 0)
		{
			face.materialIndex = materialIndex;
			mesh.faces.push_back(face);
			materialIndex++;
		}
		face.position[vertexNum] = vertices[i].position;
	}

	// インデックス数
	int numI = stacks * slices * 2 * 3;
	// インデックス設定
	u_int* indices = new u_int[numI];
	for (int y = 0; y < stacks; y++) {
		for (int x = 0; x < slices; x++) {
			int face = (y * slices + x);
			int vertices_index = y * (slices + 1) + x;
			indices[face * 6] = vertices_index + 1;
			indices[face * 6 + 1] = vertices_index;
			indices[face * 6 + 2] = vertices_index + (slices + 1);

			indices[face * 6 + 3] = vertices_index + 1;
			indices[face * 6 + 4] = vertices_index + (slices + 1);
			indices[face * 6 + 5] = vertices_index + (slices + 1) + 1;
		}
	}

	CreateBuffers(device, vertices, numV, indices, numI);

	delete[] vertices;
	delete[] indices;
}

int TexMesh::RayPick(const XMFLOAT3& startPosition, const XMFLOAT3& endPosition, XMFLOAT3* outPosition, XMFLOAT3* outNormal, float* outLength)
{
	int ret = -1;
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);
	float neart;
	DirectX::XMStoreFloat(&neart, length);

	DirectX::XMVECTOR position, normal;

	for (const auto& face : mesh.faces)
	{
		//面頂点取得
		DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&face.position[0]);
		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&face.position[1]);
		DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&face.position[2]);
		//3辺算出
		DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
		DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
		DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);
		//外積による法線算出
		DirectX::XMVECTOR n = DirectX::XMVector3Cross(ab, bc);
		//内積の結果がプラスならば裏向き
		DirectX::XMVECTOR dot = DirectX::XMVector3Dot(dir, n);
		float fdot;
		DirectX::XMStoreFloat(&fdot, dot);
		if (fdot >= 0)continue;
		//交点算出
		DirectX::XMVECTOR cp;
		DirectX::XMVECTOR as = DirectX::XMVectorSubtract(a, start);
		DirectX::XMVECTOR d1 = DirectX::XMVector3Dot(as, n);
		DirectX::XMVECTOR x = DirectX::XMVectorDivide(d1, dot);//割り算
															   //距離がnearより遠いか判定
		float fleng;
		DirectX::XMVECTOR face_leng = DirectX::XMVector3Length(x);

		DirectX::XMStoreFloat(&fleng, x);
		if (neart * neart < fleng * fleng)continue;

		cp = DirectX::XMVectorAdd(start, DirectX::XMVectorMultiply(dir, x));//足し算、かけ算
																			//内点判定
		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(a, cp);
		DirectX::XMVECTOR temp = DirectX::XMVector3Cross(v1, ab);
		DirectX::XMVECTOR work = DirectX::XMVector3Dot(temp, n);
		float fwork;
		DirectX::XMStoreFloat(&fwork, work);
		if (fwork < 0.0f)continue;
		DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(b, cp);
		temp = DirectX::XMVector3Cross(v2, bc);
		work = DirectX::XMVector3Dot(temp, n);
		DirectX::XMStoreFloat(&fwork, work);
		if (fwork < 0.0f)continue;
		DirectX::XMVECTOR v3 = DirectX::XMVectorSubtract(c, cp);
		temp = DirectX::XMVector3Cross(v3, ca);
		work = DirectX::XMVector3Dot(temp, n);
		DirectX::XMStoreFloat(&fwork, work);
		if (fwork < 0.0f)continue;

		//情報保存
		position = cp;
		normal = n;
		ret = face.materialIndex;
		neart = fleng;
	}

	if (ret != -1)
	{
		DirectX::XMStoreFloat3(outPosition, position);
		DirectX::XMStoreFloat3(outNormal, normal);
	}
	*outLength = neart;
	return ret;

}

void TexMesh::SetCbMatrix(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	cbMatrix->data.world = world;
	DirectX::XMMATRIX w, v, p, wvp;
	w = DirectX::XMLoadFloat4x4(&world);
	v = DirectX::XMLoadFloat4x4(&view);
	p = DirectX::XMLoadFloat4x4(&projection);
	wvp = w * v * p;
	DirectX::XMStoreFloat4x4(&cbMatrix->data.WVP, wvp);

	cbMatrix->Activate(context, 1);
}

void TexMesh::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, bool useShadow, const DirectX::XMFLOAT4& color)
{
	sampler->Activate(context, 0);
	texture->Set(context, 0);

	//if (!useShadow) shader->Activate(context);

	//else shadow->Activate(context);

	shader->Activate(context);

	cbColor->data.materialColor = color;
	cbColor->Activate(context, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(mesh.numI, 0, 0);

	sampler->Deactivate(context);
	cbColor->DeActivate(context);
	shader->DeActivate(context);
}

TexPlain::TexPlain(ID3D11Device* device, const wchar_t* filename, int row, int col) : TexMesh(device, filename)
{
	//頂点を定義
	Vertex* vertices = new Vertex[(row + 1) * (col + 1)];
	float w = 1.0f / col;
	float h = 1.0f / row;
	for (int y = 0; y < row + 1; y++)
	{
		for (int x = 0; x < col + 1; x++)
		{
			int index = (col + 1) * y + x;
			vertices[index].position.x = w * x - 0.5f;
			vertices[index].position.y = h * y - 0.5f;
			vertices[index].position.z = 0.0f;

			vertices[index].texcoord.x = w * x;
			vertices[index].texcoord.y = 1.0f - h * y;
			vertices[index].normal = VECTOR3(0.f, 0.f, 1.f);
			vertices[index].color = VECTOR4(1.f, 1.f, 1.f, 1.f);
		}
	}
	int numV = (row + 1) * (col + 1);

	//const int size = ;
	UINT16* indices = new UINT16[row * col * 3 * 2];
	int n = 0;
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			indices[n + 0] = (col + 1) * y + x;
			indices[n + 1] = (col + 1) * y + x + 1;
			indices[n + 2] = (col + 1) * (y + 1) + x;

			indices[n + 3] = (col + 1) * (y + 1) + x;
			indices[n + 4] = (col + 1) * y + x + 1;
			indices[n + 5] = (col + 1) * (y + 1) + (x + 1);
			n += 6;
		}
	}
	int numI = row * col * 3 * 2;

	HRESULT hr;

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	mesh.numV = numV;
	{
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Vertex) * mesh.numV;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		//desc.StructureByteStride = 0;

		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertices;
		//data.SysMemPitch = 0;
		//data.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(&desc, &data, mesh.vertexBuffer.GetAddressOf());

		if (FAILED(hr)) assert(!"vertexBuffer");
	}

	mesh.numI = numI;
	{
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(u_int) * mesh.numI;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		//desc.StructureByteStride = 0;

		ZeroMemory(&data, sizeof(data));
		data.pSysMem = indices;
		//data.SysMemPitch = 0;
		//data.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(&desc, &data, mesh.indexBuffer.GetAddressOf());
		if (FAILED(hr)) assert(!"indexBuffer");
	}

	delete[] vertices;
	delete[] indices;
}

//TexPlain::TexPlain(ID3D11Device* device, std::shared_ptr<Texture>& texture, int row, int col) : TexMesh(device, texture)
//{
//	//頂点を定義
//	Vertex* vertices = new Vertex[(row + 1) * (col + 1)];
//	float w = 1.0f / col;
//	float h = 1.0f / row;
//	for (int y = 0; y < row + 1; y++) {
//		for (int x = 0; x < col + 1; x++) {
//			int index = (col + 1) * y + x;
//			vertices[index].position.x = w * x - 0.5f;
//			vertices[index].position.y = h * y - 0.5f;
//			vertices[index].position.z = 0.0f;
//
//			vertices[index].texcoord.x = w * x;
//			vertices[index].texcoord.y = 1.0f - h * y;
//			vertices[index].normal = VECTOR3(0, 0, 1);
//			vertices[index].color = VECTOR4(1, 1, 1, 1);
//		}
//	}
//	int numV = (row + 1) * (col + 1);
//
//	u_int* indices = new u_int[row * col * 3 * 2];
//	int n = 0;
//	for (int y = 0; y < row; y++) {
//		for (int x = 0; x < col; x++) {
//			indices[n + 0] = (col + 1) * y + x;
//			indices[n + 1] = (col + 1) * y + x + 1;
//			indices[n + 2] = (col + 1) * (y + 1) + x;
//
//			indices[n + 3] = (col + 1) * (y + 1) + x;
//			indices[n + 4] = (col + 1) * y + x + 1;
//			indices[n + 5] = (col + 1) * (y + 1) + (x + 1);
//			n += 6;
//		}
//	}
//	int numI = row * col * 3 * 2;
//
//	HRESULT hr;
//
//	D3D11_BUFFER_DESC desc;
//	D3D11_SUBRESOURCE_DATA data;
//
//	mesh.numV = numV;
//	{
//		ZeroMemory(&desc, sizeof(desc));
//		desc.Usage = D3D11_USAGE_DEFAULT;
//		desc.ByteWidth = sizeof(Vertex) * mesh.numV;
//		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		desc.CPUAccessFlags = 0;
//		desc.MiscFlags = 0;
//
//		ZeroMemory(&data, sizeof(data));
//		data.pSysMem = vertices;//変更
//		hr = device->CreateBuffer(&desc, &data, mesh.vertexBuffer.GetAddressOf());
//
//		if (FAILED(hr)) assert(!"vertexBuffer");
//	}
//
//	mesh.numI = numI;
//	{
//		ZeroMemory(&desc, sizeof(desc));
//		desc.Usage = D3D11_USAGE_DEFAULT;
//		desc.ByteWidth = sizeof(u_int) * mesh.numI;
//		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//		desc.CPUAccessFlags = 0;
//		desc.MiscFlags = 0;
//
//		ZeroMemory(&data, sizeof(data));
//		data.pSysMem = indices;
//		hr = device->CreateBuffer(&desc, &data, mesh.indexBuffer.GetAddressOf());
//		if (FAILED(hr)) assert(!"indexBuffer");
//	}
//
//	delete[] vertices;
//	delete[] indices;
//}

//TexPlain::TexPlain(ID3D11Device* device, const wchar_t* filename) : TexMesh(device, filename)
//{
//	//Vertex vertices[4] = {};
//	//u_int indices[6] = {};
//
//	//vertices[0].position = VECTOR3(-0.5f, +0.5f, .0f);
//	//vertices[1].position = VECTOR3(+0.5f, +0.5f, .0f);
//	//vertices[2].position = VECTOR3(-0.5f, -0.5f, .0f);
//	//vertices[3].position = VECTOR3(+0.5f, -0.5f, .0f);
//
//	//vertices[0].normal = vertices[1].normal =
//	//	vertices[2].normal =
//	//	vertices[3].normal = VECTOR3(+0.0f, +0.0f, -1.0f);
//
//	//vertices[0].texcoord = VECTOR2(1.f, 0.f);
//	//vertices[1].texcoord = VECTOR2(0.f, 0.f);
//	//vertices[2].texcoord = VECTOR2(1.f, 1.f);
//	//vertices[3].texcoord = VECTOR2(0.f, 1.f);
//
//	//vertices[0].color = vertices[1].color =
//	//	vertices[2].color =
//	//	vertices[3].color = VECTOR4(1.f, 1.f, 1.f, 1.f);
//
//	//indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
//	//indices[3] = 1;	indices[4] = 3;	indices[5] = 2;
//
//	//CreateBuffers(device, vertices, 4, indices, 6);
//}
//
//TexPlain::TexPlain(ID3D11Device* device, std::shared_ptr<Texture>& texture) : TexMesh(device, texture)
//{
//}

TexBoard::TexBoard(ID3D11Device* device, const wchar_t* filename, const VECTOR2& texPos, const VECTOR2& texSize) : TexMesh(device, filename)
{
	Vertex vertices[4] = {};
	u_int indices[3 * 2] = {};

	vertices[0].position = VECTOR3(-0.5f, +0.5f, .0f);
	vertices[1].position = VECTOR3(+0.5f, +0.5f, .0f);
	vertices[2].position = VECTOR3(-0.5f, -0.5f, .0f);
	vertices[3].position = VECTOR3(+0.5f, -0.5f, .0f);

	Face face;
	int materialIndex = 0;
	for (int i = 0; i < 4; i++)
	{
		int vertexNum = i % 3;
		if ((vertexNum == 0) && i != 0)
		{
			face.materialIndex = materialIndex;
			mesh.faces.push_back(face);
			materialIndex++;
		}
		face.position[vertexNum] = vertices[i].position;
	}

	vertices[0].normal = vertices[1].normal =
		vertices[2].normal =
		vertices[3].normal = VECTOR3(+0.0f, +0.0f, -1.0f);

	const float sx = texPos.x, sy = texPos.y;
	const float sw = texSize.x, sh = texSize.y;

	vertices[0].texcoord = VECTOR2(sx, sy);
	vertices[1].texcoord = VECTOR2(sx + sw, sy);
	vertices[2].texcoord = VECTOR2(sx, sy + sh);
	vertices[3].texcoord = VECTOR2(sx + sw, sy + sh);
	for (int i = 0; i < 4; i++)
	{
		vertices[i].texcoord.x = vertices[i].texcoord.x / texture->GetWidth();
		vertices[i].texcoord.y = vertices[i].texcoord.y / texture->GetHeight();
	}

	vertices[0].color = vertices[1].color =
		vertices[2].color =
		vertices[3].color = VECTOR4(1.f, 1.f, 1.f, 1.f);

	const int numV = sizeof(vertices) / sizeof(vertices[0]);

	indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
	indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

	const int numI = sizeof(indices) / sizeof(indices[0]);

	// 96        24
	CreateBuffers(device, vertices, numV, indices, numI);
}

TexBoard::TexBoard(ID3D11Device* device, std::shared_ptr<Texture>& texture) : TexMesh(device, texture)
{
	Vertex vertices[4] = {};
	u_int indices[3 * 2] = {};

	vertices[0].position = VECTOR3(-0.5f, +0.5f, .0f);
	vertices[1].position = VECTOR3(+0.5f, +0.5f, .0f);
	vertices[2].position = VECTOR3(-0.5f, -0.5f, .0f);
	vertices[3].position = VECTOR3(+0.5f, -0.5f, .0f);

	Face face[2];
	face[0].position[0] = vertices[0].position;
	face[0].position[1] = vertices[2].position;
	face[0].position[2] = vertices[1].position;
	face[0].materialIndex = 0;
	mesh.faces.push_back(face[0]);

	face[1].position[0] = vertices[3].position;
	face[1].position[1] = vertices[2].position;
	face[1].position[2] = vertices[0].position;
	face[1].materialIndex = 1;
	mesh.faces.push_back(face[1]);

	//int materialIndex = 0;
	//for (int i = 0; i < 4; i++)
	//{
	//	int vertexNum = i % 3;
	//	if ((vertexNum == 0) && i != 0)
	//	{
	//		face.materialIndex = materialIndex;
	//		mesh.faces.push_back(face);
	//		materialIndex++;
	//	}
	//	face.position[vertexNum] = vertices[i].position;
	//}

	vertices[0].normal = vertices[1].normal =
		vertices[2].normal =
		vertices[3].normal = VECTOR3(+0.0f, +0.0f, -1.0f);

	vertices[0].texcoord = VECTOR2(-1.f, 0.f);
	vertices[1].texcoord = VECTOR2(0.f, 0.f);
	vertices[2].texcoord = VECTOR2(-1.f, 1.f);
	vertices[3].texcoord = VECTOR2(0.f, 1.f);

	vertices[0].color = vertices[1].color =
		vertices[2].color =
		vertices[3].color = VECTOR4(1.f, 1.f, 1.f, 1.f);

	indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
	indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

	CreateBuffers(device, vertices, 4, indices, 3 * 2);
}

TexTrajectory::TexTrajectory(ID3D11Device* device, const wchar_t* filename) : TexMesh(device, filename)
{


}

TexTrajectory::TexTrajectory(ID3D11Device* device, std::shared_ptr<Texture>& texture) : TexMesh(device, texture)
{


}
