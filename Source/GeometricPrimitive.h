#pragma once
#include<d3d11.h>
#include <DirectXMath.h>

class GeometricPrimitive
{
private:
	ID3D11VertexShader* vertex_shader = nullptr;
	ID3D11VertexShader* null_vertex_shader = nullptr;

	ID3D11PixelShader* pixel_shader = nullptr;
	ID3D11PixelShader* null_pixel_shader = nullptr;

	ID3D11InputLayout* input_layout = nullptr;

	ID3D11Buffer* vertex_buffer = nullptr;
	ID3D11Buffer* index_buffer = nullptr;
	ID3D11Buffer* constant_buffer = nullptr;
	ID3D11Buffer* null_constant_buffer = nullptr;

	ID3D11DepthStencilState* 	depthStencilState=nullptr;
	ID3D11RasterizerState* rasterizer_state = nullptr;
	int num;
public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4 light_direction;
	};
protected:
	void createbuffers(ID3D11Device* device, vertex* vertex_, int vertex_num, u_int* index_, int index_num);
public:
	GeometricPrimitive(ID3D11Device* device) {};
	virtual ~GeometricPrimitive()
	{
		if (vertex_shader != nullptr)
		{
			vertex_shader->Release();
			vertex_shader = nullptr;
		}
		if (pixel_shader != nullptr)
		{
			pixel_shader->Release();
			pixel_shader = nullptr;
		}
		if (input_layout != nullptr)
		{
			input_layout->Release();
			input_layout = nullptr;
		}
		if (vertex_buffer != nullptr)
		{
			vertex_buffer->Release();
			vertex_buffer = nullptr;
		}
		if (index_buffer != nullptr)
		{
			index_buffer->Release();
			index_buffer = nullptr;
		}
		if (constant_buffer != nullptr)
		{
			constant_buffer->Release();
			constant_buffer = nullptr;
		}
		if (depthStencilState != nullptr)
		{
			depthStencilState->Release();
			depthStencilState = nullptr;
		}
		if (rasterizer_state != nullptr)
		{
			rasterizer_state->Release();
			rasterizer_state = nullptr;
		}
	}
	void render(ID3D11DeviceContext* immediate_context,
		const DirectX::XMFLOAT4& color,
		const DirectX::XMFLOAT4& light_direction,
		const DirectX::XMFLOAT4X4& world_view_projection,
		const DirectX::XMFLOAT4X4& world,
		const bool topologyFlag=false) const;
};

class GeometricCube :public GeometricPrimitive
{
public:
	GeometricCube(ID3D11Device* device);
};
class GeometricCylinder :public GeometricPrimitive
{
public:
	GeometricCylinder(ID3D11Device* device, u_int slices);//slices=32
};
class GeometricSphere :public GeometricPrimitive
{
public:
	GeometricSphere(ID3D11Device* device, u_int slices, u_int stacks);//slices=32,stacks=16
};
class GeometricCapsule :public GeometricPrimitive
{
public:
	GeometricCapsule(ID3D11Device* device);
};