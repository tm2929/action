#pragma once
#pragma once
#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "vector.h"
#include "constant_buffer.h"
#include "texture.h"
#include "sampler.h"
#include "shader.h"


class TexMesh
{
public:
	TexMesh(ID3D11Device* device, const wchar_t* filename)
	{
		shader = std::make_unique<Shader>();
		shader->CreateTexGeometric(device);

		//shadow = std::make_unique<Shader>();
		//shadow->CreateMeshShadow(device);

		sampler = std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);

		texture = std::make_shared<Texture>();
		texture->Load(device, filename);

		cbColor = std::make_unique<ConstantBuffer<CBColor>>(device);
		cbMatrix = std::make_unique<ConstantBuffer<CBMatrix>>(device);
	}
	TexMesh(ID3D11Device* device, std::shared_ptr<Texture>& texture)
	{
		shader = std::make_unique<Shader>();
		shader->CreateTexGeometric(device);

		//shader = std::make_unique<Shader>();
		//shadow->CreateMeshShadow(device);

		sampler = std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);

		this->texture = texture;

		cbColor = std::make_unique<ConstantBuffer<CBColor>>(device);
		cbMatrix = std::make_unique<ConstantBuffer<CBMatrix>>(device);
	}
	virtual ~TexMesh() {}

	void SetTexture(std::shared_ptr<Texture>& texture) { this->texture = texture; }
	virtual int RayPick(const XMFLOAT3& startPosition, const XMFLOAT3& endPosition,
		XMFLOAT3* outPosition, XMFLOAT3* outNormal, float* outLength);
	void SetCbMatrix(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);
	void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, bool useShadow, const DirectX::XMFLOAT4& color);

protected:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texcoord;
		XMFLOAT4 color;
	};

	struct Face
	{
		XMFLOAT3 position[3];
		int materialIndex;
	};
	struct Mesh
	{
		ComPtr<ID3D11Buffer> vertexBuffer;
		int numV;
		ComPtr<ID3D11Buffer> indexBuffer;
		int numI;

		std::vector<Face> faces;
	};
	Mesh mesh = {};
	void CreateBuffers(ID3D11Device* device, Vertex* vertices, int numV, u_int* indices, int numI);

	struct CBColor
	{
		DirectX::XMFLOAT4 materialColor;
	};
	std::unique_ptr<ConstantBuffer<CBColor>> cbColor;
	struct CBMatrix
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 WVP;
	};
	std::unique_ptr<ConstantBuffer<CBMatrix>> cbMatrix;

	std::unique_ptr<Shader> shader;
	std::unique_ptr<Shader> shadow;
	std::unique_ptr<Sampler> sampler;
	std::shared_ptr<Texture> texture;
};

class TexCube : public TexMesh
{
public:
	TexCube(ID3D11Device* device, const wchar_t* filename);
	TexCube(ID3D11Device* device, std::shared_ptr<Texture>& texture);
	~TexCube() {}
};

class TexPlain : public TexMesh
{
public:
	TexPlain(ID3D11Device* device, const wchar_t* filename, int row = 5, int col = 5);
	//TexPlain(ID3D11Device* device, std::shared_ptr<Texture>& texture, int row = 5, int col = 5);
	//TexPlain(ID3D11Device* device, const wchar_t* filename);
	//TexPlain(ID3D11Device* device, std::shared_ptr<Texture>& texture);
	~TexPlain() {}
};

class TexSphere : public TexMesh
{
public:
	TexSphere(ID3D11Device* device, const wchar_t* filename, int slices = 8, int stacks = 8);
	TexSphere(ID3D11Device* device, std::shared_ptr<Texture>& texture, int slices = 8, int stacks = 8);
	~TexSphere() {}
};

class TexBoard : public TexMesh
{
public:
	TexBoard(ID3D11Device* device, const wchar_t* filename, const VECTOR2& texPos, const VECTOR2& texSize);
	TexBoard(ID3D11Device* device, std::shared_ptr<Texture>& texture);
	~TexBoard() {}
};
class TexTrajectory : public TexMesh
{

public:
	TexTrajectory(ID3D11Device* device, const wchar_t* filename);
	TexTrajectory(ID3D11Device* device, std::shared_ptr<Texture>& texture);
	~TexTrajectory() {}
};