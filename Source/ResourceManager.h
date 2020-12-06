//#pragma once
//#include <windows.h>
//#include<d3d11.h>
//#include <map>
//#include <string>
//class ResourceManager
//{
//public:
//	HRESULT LoadTexture(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC* desc);
//	HRESULT LoadPixelShader(ID3D11Device* device, const wchar_t* filename, ID3D11PixelShader** pis);
//	HRESULT LoadVertexShader(ID3D11Device* device, const wchar_t* filename, ID3D11VertexShader** ves, ID3D11InputLayout** input, D3D11_INPUT_ELEMENT_DESC* input_elemen, UINT numElements);
//
//private:
//	ResourceManager() {};
//public:
//	~ResourceManager() { Release(); };
//
//	static ResourceManager& getinctanse()
//	{
//		static ResourceManager r_manager;
//		return r_manager;
//	}
//	void Release();
//private:
//	struct TextureInfo
//	{
//		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
//		D3D11_TEXTURE2D_DESC desc;
//	};
//	struct Vertexinput
//	{
//		Microsoft::WRL::ComPtr<ID3D11VertexShader>Vshader;
//		Microsoft::WRL::ComPtr<ID3D11InputLayout>InputL;
//	};
//
//	std::map<std::wstring, TextureInfo> mp;
//	std::map<std::wstring, Vertexinput> vp;
//	std::map<std::wstring, ID3D11PixelShader*> pp;
//
//	//TextureInfo info[1024];
//	int info_count;
//};
//#define pResourceManager (ResourceManager::getinctanse())
//
