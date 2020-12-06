#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>

#include "TexMeshObj.h"
class SkayMap
{
public:
	SkayMap(ID3D11Device* device, const char* cubemap_filename);
	void render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& colour);

private:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		
		vertex() = default;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive
			(
				position.x, position.y, position.z
			);
		}
	};
	

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	struct shader_constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 colour;
	};
public:
	D3D11_TEXTURE2D_DESC texture2d_desc;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;



};