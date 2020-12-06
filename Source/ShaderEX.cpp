#include "shader.h"
#include "misc.h"

#include<memory>

#include<map>
#include <wrl.h>
#include<string>

HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements, bool enableCaching)
{
	struct set_of_vertex_shader_and_input_layout
	{
		set_of_vertex_shader_and_input_layout(ID3D11VertexShader* vertex_shader, ID3D11InputLayout* input_layout) : vertex_shader(vertex_shader), input_layout(input_layout) {}
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	};
	static std::map<std::string, set_of_vertex_shader_and_input_layout> cache;

	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*vertex_shader = it->second.vertex_shader.Get();
		(*vertex_shader)->AddRef();
		*input_layout = it->second.input_layout.Get();
		(*input_layout)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	if (input_layout)
	{
		hr = device->CreateInputLayout(input_element_desc, num_elements, cso_data.get(), cso_sz, input_layout);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	if (enableCaching)
	{
		cache.insert(std::make_pair(cso_name, set_of_vertex_shader_and_input_layout(*vertex_shader, *input_layout)));
	}


	return hr;
}

HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		//it->second.Attach(*pixel_shader);
		*pixel_shader = it->second.Get();
		(*pixel_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	cache.insert(std::make_pair(cso_name, *pixel_shader));

	return hr;
}

HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11GeometryShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		//it->second.Attach(*pixel_shader);
		*geometry_shader = it->second.Get();
		(*geometry_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateGeometryShader(cso_data.get(), cso_sz, nullptr, geometry_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	cache.insert(std::make_pair(cso_name, *geometry_shader));

	return hr;
}

HRESULT create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11ComputeShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		//it->second.Attach(*pixel_shader);
		*compute_shader = it->second.Get();
		(*compute_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateComputeShader(cso_data.get(), cso_sz, nullptr, compute_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	cache.insert(std::make_pair(cso_name, *compute_shader));

	return hr;
}

//HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* file_name, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
//{
//	HRESULT hr = S_OK;
//	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
//	static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;
//	auto it = cache.find(file_name);
//	if (it != cache.end())
//	{
//		//it->second.Attach(*shader_resource_view);
//		*shader_resource_view = it->second.Get();
//		(*shader_resource_view)->AddRef();
//		(*shader_resource_view)->GetResource(resource.GetAddressOf());
//	}
//	else
//	{
//		// UNIT.04
//		hr = DirectX::CreateWICTextureFromFile(device, file_name, resource.GetAddressOf(), shader_resource_view);
//		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
//		cache.insert(std::make_pair(file_name, *shader_resource_view));
//	}
//	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
//	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
//	texture2d->GetDesc(texture2d_desc);
//
//	return hr;
//}
