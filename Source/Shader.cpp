
#include "shader.h"
#include "ResourceManager.h"
#include "ShaderEX.h"
Shader::Shader(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& immediateContext)
{
	this->device = device;
	this->immediateContext = immediateContext;
}

bool Shader::Create(ID3D11Device* device, ID3D11DeviceContext* device_context, char * vsName, char* psName, UINT numElements , D3D11_INPUT_ELEMENT_DESC* input_elemen)
{
	create_vs_from_cso(device, vsName, &VS, &VertexLayout, input_elemen, numElements);

	create_ps_from_cso(device, psName, PS.GetAddressOf());

	return true;
}

bool Shader::CreateSprite(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	const char * vsName = "Data/shaders/cso/sprite_vs.cso";
	const char * psName = "Data/shaders/cso/sprite_ps.cso";

	create_vs_from_cso(device, vsName, &VS, &VertexLayout, layout,numElements);

	create_ps_from_cso(device, psName, PS.GetAddressOf());

	return true;
}

bool Shader::CreateSpriteBatch(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC	layout[] =
	{
		{ "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "NDC_TRANSFORM",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR",              0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	UINT numElements = ARRAYSIZE(layout);
	const char* vsName = "Data/shaders/cso/sprite_batch_vs.cso";
	const char* psName = "Data/shaders/cso/sprite_ps.cso";

	create_vs_from_cso(device, vsName, &VS, &VertexLayout, layout,numElements);

	create_ps_from_cso(device, psName, PS.GetAddressOf());

	return true;
}

bool Shader::CreateBoard(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC layout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	const char* vsName = "Data/shaders/cso/board_vs.cso";
	const char* psName = "Data/shaders/cso/board_ps.cso";

	create_vs_from_cso(device, vsName, &VS, &VertexLayout, layout, numElements);

	create_ps_from_cso(device, psName, PS.GetAddressOf());

	return true;
}

bool Shader::CreateGeometric(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	const char* vsName ="Data/shaders/cso/geometric_primitive_vs.cso";
	const char* psName ="Data/shaders/cso/geometric_primitive_ps.cso";

	create_vs_from_cso(device, vsName, &VS, &VertexLayout, layout, numElements);
	create_ps_from_cso(device, psName, PS.GetAddressOf());

	return true;
}

bool Shader::CreateModel(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	const char* vsName = "Data/shaders/cso/model_vs.cso";
	const char* psName = "Data/shaders/cso/model_ps.cso";

	create_vs_from_cso(device, vsName, &VS, &VertexLayout, layout, numElements);

	create_ps_from_cso(device, psName, PS.GetAddressOf());

	return true;
}

bool Shader::CreateMesh(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	const char* vsName = "Data/shaders/cso/3DObj_vs.cso";
	const char* psName = "Data/shaders/cso/3DObj_ps.cso";

	create_vs_from_cso(device, vsName, &VS, &VertexLayout, layout, numElements);

	create_ps_from_cso(device, psName, PS.GetAddressOf());

	return true;
}
bool Shader::CreateObj(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	const char* vsName = "Data/shaders/cso/meshs_vs.cso";
	const char* psName = "Data/shaders/cso/meshs_ps.cso";

	create_vs_from_cso(device, vsName, VS.GetAddressOf(), &VertexLayout, layout, numElements);

	create_ps_from_cso(device, psName, PS.GetAddressOf());

	return true;
}
bool Shader::CreateTexGeometric(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC layout[] = {
	 { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	UINT numElements = ARRAYSIZE(layout);

	const char* vsName = "Data/shaders/cso/meshs_vs.cso";
	//const char* gsName = "Data/shaders/cso/CubeMap_gs.cso";
	const char* psName = "Data/shaders/cso/meshs_ps.cso";

	create_vs_from_cso(device, vsName, VS.GetAddressOf(), VertexLayout.GetAddressOf(), layout, numElements);
//	create_gs_from_cso(device, gsName, GS.GetAddressOf());
	create_ps_from_cso(device, psName, PS.GetAddressOf());

	return true;
}
void Shader::Activate(ID3D11DeviceContext* context)
{
	context->IASetInputLayout(VertexLayout.Get());

	context->VSSetShader(VS.Get(), NULL, 0);
	context->PSSetShader(PS.Get(), NULL, 0);
	context->GSSetShader(GS.Get(), NULL, 0);
	context->HSSetShader(HS.Get(), NULL, 0);
	context->DSSetShader(DS.Get(), NULL, 0);
}

void Shader::DeActivate(ID3D11DeviceContext* context)
{
	

	context->VSSetShader(NULL, NULL, 0);
	context->HSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
	context->GSSetShader(NULL, NULL, 0);
	context->PSSetShader(NULL, NULL, 0);
}
