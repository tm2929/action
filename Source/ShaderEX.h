#pragma once
#include <d3d11.h>
#include "Shader.h"
HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements, bool enableCaching=true);
HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);
HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);
HRESULT create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader);

class ShaderEx :public Shader
{
public:
	ShaderEx() {}
	virtual ~ShaderEx() {}
	bool Create(ID3D11Device* device,const char* vsfilename, const char* psfilename);
	bool Create(ID3D11Device* device, WCHAR* vsfilename, WCHAR* gsfilename, WCHAR* psfilename);
	bool Create(ID3D11Device* device, WCHAR* vsfilename, WCHAR* dsfilename, WCHAR* hsfilename, WCHAR* psfilename);
	bool Create(ID3D11Device* device, WCHAR* vsfilename, WCHAR* dsfilename, WCHAR* hsfilename, WCHAR* gsfilename, WCHAR* psfilename);


};
