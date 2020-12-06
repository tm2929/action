#include "logger.h"
#include "ModelRenderer.h"
#include "misc.h"
#include <functional>
#include "FbxLoad.h"
#include <fstream>
#include <iostream>
#include "ShaderEX.h"

ModelRenderer::ModelRenderer(ID3D11Device* device, bool depthFlag)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HRESULT	hr = create_vs_from_cso(device, "Data/shaders/cso/model_vs.cso", vertexShader.GetAddressOf(), &inputLayout, input_element_desc, ARRAYSIZE(input_element_desc));
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	//シャドウ

	{
		/*create_vs_from_cso(device, "Data/shaders/cso/modelShadow_vs.cso", shadowShader.GetAddressOf(), shadow_input.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));*/
		hr = create_vs_from_cso(device, "Data/shaders/cso/modelShadow_vs.cso", shadowShader.GetAddressOf(), shadow_input.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ピクセルシェーダー

	{
		hr = create_ps_from_cso(device,"Data/shaders/cso/model_ps.cso", pixelShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 定数バッファ
	{
		// シーン用バッファ
		D3D11_BUFFER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CbScene);
		desc.StructureByteStride = 0;

		hr = device->CreateBuffer(&desc, 0, cbScene.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = device->CreateBuffer(&desc, 0, cbObj.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// メッシュ用バッファ
		desc.ByteWidth = sizeof(CbMesh);

		hr = device->CreateBuffer(&desc, 0, cbMesh.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// サブセット用バッファ
		desc.ByteWidth = sizeof(CbSubset);

		hr = device->CreateBuffer(&desc, 0, cbSubset.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ブレンドステート
	/*{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}*/

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;//深度値をみるかみないか
		if (depthFlag)
		{
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;//zero 書き込まない　
		}
		else
		{
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		}
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	//// ラスタライザーステート
	//{
	//	D3D11_RASTERIZER_DESC desc;
	//	::memset(&desc, 0, sizeof(desc));
	//	desc.FrontCounterClockwise = true;
	//	desc.DepthBias = 0;
	//	desc.DepthBiasClamp = 0;
	//	desc.SlopeScaledDepthBias = 0;
	//	desc.DepthClipEnable = true;
	//	desc.ScissorEnable = false;
	//	desc.MultisampleEnable = true;
	//	desc.FillMode = D3D11_FILL_SOLID;
	//	desc.CullMode = D3D11_CULL_FRONT;
	//	desc.AntialiasedLineEnable = false;

	//	HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	//}

	// サンプラステート
	/*{
		D3D11_SAMPLER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		HRESULT hr = device->CreateSamplerState(&desc, m_sampler_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}*/

	// ダミーテクスチャ
	{
		const int width = 8;
		const int height = 8;
		UINT pixels[width * height];
		::memset(pixels, 0xFF, sizeof(pixels));

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA data;
		::memset(&data, 0, sizeof(data));
		data.pSysMem = pixels;
		data.SysMemPitch = width;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
		HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = device->CreateShaderResourceView(texture.Get(), nullptr, dummySrv.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

ModelRenderer::~ModelRenderer()
{
	cbScene = nullptr;
	cbObj = nullptr;
	cbMesh = nullptr;
	cbSubset = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	shadowShader = nullptr;
	inputLayout = nullptr;
	shadow_input = nullptr;
	blendState = nullptr;
	rasterizerState = nullptr;
	rasterizerState02 = nullptr;
	depthStencilState = nullptr;
	dummySrv = nullptr;
}

// 描画開始
void ModelRenderer::Begin(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& light_direction)
{
	context->VSSetShader(vertexShader.Get(), nullptr, 0);
	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	context->IASetInputLayout(inputLayout.Get());
	ID3D11Buffer* constant_buffers[] =
	{
		cbScene.Get(),
		cbMesh.Get(),
		cbSubset.Get()
	};
	context->VSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
	context->PSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
	context->OMSetDepthStencilState(depthStencilState.Get(), 0);
	// シーン用定数バッファ更新
	CbScene cb_scene;
	cb_scene.view_projection = view_projection;
	cb_scene.light_direction = light_direction;
	context->UpdateSubresource(cbScene.Get(), 0, 0, &cb_scene, 0, 0);
}

// 描画
void ModelRenderer::Draw(ID3D11DeviceContext* context, Model* model, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT4& lightObj)
{
	const ModelResource* model_resource = model->GetResource();
	const std::vector<Model::Node>& nodes = model->GetNodes();

	for (const ModelResource::Mesh& mesh : model_resource->GetMeshes())
	{
		// メッシュ用定数バッファ更新
		CbMesh cb_mesh;
		::memset(&cb_mesh, 0, sizeof(cb_mesh));
		if (mesh.nodeIndices.size() > 0)
		{
			for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
			{
				DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
				DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
				DirectX::XMMATRIX bone_transform = offsetTransform * world_transform;
				DirectX::XMStoreFloat4x4(&cb_mesh.bone_transforms[i], bone_transform);
			}
		}
		else
		{
			cb_mesh.bone_transforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
		}
		context->UpdateSubresource(cbMesh.Get(), 0, 0, &cb_mesh, 0, 0);

		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			CbSubset cb_subset;
			cb_subset.material_color = VECTOR4(subset.material->color.x * color.x, subset.material->color.y * color.y, subset.material->color.z * color.z, subset.material->color.w * color.w);
			context->UpdateSubresource(cbSubset.Get(), 0, 0, &cb_subset, 0, 0);
			context->PSSetShaderResources(0, 1, subset.material->shaderResourceView.Get() ? subset.material->shaderResourceView.GetAddressOf() : dummySrv.GetAddressOf());
			//	context->PSSetSamplers(0, 1, m_sampler_state.GetAddressOf());
			context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
		}
	}

}

// 描画終了
void ModelRenderer::End(ID3D11DeviceContext* context)
{

}

void ModelRenderer::ShadowBegin(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view_projection)
{
	context->VSSetShader(shadowShader.Get(), nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
	context->IASetInputLayout(inputLayout.Get());

	ID3D11Buffer* constant_buffers[] =
	{
		cbScene.Get(),
		cbMesh.Get(),
		cbSubset.Get()
	};
	context->VSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
	context->PSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);

	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetDepthStencilState(depthStencilState.Get(), 0);
	//context->RSSetState(rasterizerState.Get());

	// シーン用定数バッファ更新
	CbScene cb_scene;
	cb_scene.view_projection = view_projection;
	cb_scene.light_direction = DirectX::XMFLOAT4(0, 0, 0, 0);


	context->UpdateSubresource(cbScene.Get(), 0, 0, &cb_scene, 0, 0);
}

void ModelRenderer::ShadowDraw(ID3D11DeviceContext* context, Model& model, const DirectX::XMFLOAT4& color)
{
	const ModelResource* model_resource = model.GetResource();
	const std::vector<Model::Node>& nodes = model.GetNodes();

	for (const ModelResource::Mesh& mesh : model_resource->GetMeshes())
	{
		// メッシュ用定数バッファ更新
		CbMesh cb_mesh;
		::memset(&cb_mesh, 0, sizeof(cb_mesh));
		if (mesh.nodeIndices.size() > 0)
		{
			for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
			{
				DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
				DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
				DirectX::XMMATRIX bone_transform = offsetTransform * worldTransform;
				DirectX::XMStoreFloat4x4(&cb_mesh.bone_transforms[i], bone_transform);
			}
		}
		else
		{
			cb_mesh.bone_transforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
		}
		context->UpdateSubresource(cbMesh.Get(), 0, 0, &cb_mesh, 0, 0);

		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			CbSubset cb_subset;
			cb_subset.material_color = VECTOR4(subset.material->color.x * color.x, subset.material->color.y * color.y, subset.material->color.z * color.z, subset.material->color.w * color.w);
			context->UpdateSubresource(cbSubset.Get(), 0, 0, &cb_subset, 0, 0);
			context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
		}
	}
}

void ModelRenderer::ShadowEnd(ID3D11DeviceContext* context)
{


}
