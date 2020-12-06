//#include"ResourceManager.h"
//#include"WICTextureLoader.h"
//#include "assert.h"
//HRESULT ResourceManager::LoadTexture(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC* desc)
//{
//	HRESULT hr = S_OK;
//
//	auto it = mp.find(filename);
//	ID3D11Resource* resource = nullptr;
//	TextureInfo tp;
//	//‚È‚©‚Á‚½‚ç
//	if (it == mp.end())
//	{
//
//		hr = DirectX::CreateWICTextureFromFile(device, filename, &resource, srv);
//		ID3D11Texture2D* tex2d;
//		hr = resource->QueryInterface(&tex2d);
//		tex2d->GetDesc(desc);
//		tp.srv = *srv;
//		tp.desc = *desc;
//		mp.emplace(filename, tp);
//
//
//	}
//	//‚ ‚Á‚½‚ç
//	else
//	{
//
//		tp = mp[filename];
//		*srv = tp.srv;
//		*desc = tp.desc;
//	}
//
//
//	//resource->Release();
//	resource = nullptr;
//	//return E_NOTIMPL;
//	return hr;
//}
//
//HRESULT ResourceManager::LoadPixelShader(ID3D11Device* device, const wchar_t* filename, ID3D11PixelShader** pis)
//{
//	HRESULT hr = S_OK;
//	auto it = pp.find(filename);
//	if (it == pp.end())
//	{
//		FILE* pf = 0;
//		_wfopen_s(&pf, filename, L"rb");//L‚ð‚Â‚¯‚é‚Æwchar_tŒ^‚É‚È‚é
//		fseek(pf, 0, SEEK_END);
//		long pcso_s = ftell(pf);
//		fseek(pf, 0, SEEK_SET);
//		unsigned char* pcso_dat = new unsigned char[pcso_s];
//		fread(pcso_dat, pcso_s, 1, pf);
//		fclose(pf);
//		hr = device->CreatePixelShader(pcso_dat, pcso_s, NULL, pis);
//		pp.emplace(filename, *pis);
//		delete[] pcso_dat;
//	}
//	else
//	{
//		*pis = pp[filename];
//	}
//	return hr;
//}
//
//HRESULT ResourceManager::LoadVertexShader(ID3D11Device* device, const wchar_t* filename, ID3D11VertexShader** ves, ID3D11InputLayout** input, UINT numElements, D3D11_INPUT_ELEMENT_DESC* input_elemen)
//{
//	HRESULT hr = S_OK;
//	auto it = vp.find(filename);
//	Vertexinput vi;
//	if (it == vp.end())
//	{
//		FILE* fp = 0;
//		_wfopen_s(&fp, filename, L"rb");
//		fseek(fp, 0, SEEK_END);
//		long cso_sz = ftell(fp);
//		fseek(fp, 0, SEEK_SET);
//		unsigned char* cso_data = new unsigned char[cso_sz];
//		fread(cso_data, cso_sz, 1, fp);
//		fclose(fp);
//		hr = device->CreateVertexShader(cso_data, cso_sz, NULL, ves);
//
//		hr = device->CreateInputLayout(input_elemen, numElements, cso_data, cso_sz, input);
//		vi.Vshader = *ves;
//		vi.InputL = *input;
//		vp.emplace(filename, vi);
//		delete[] cso_data;
//	}
//	else
//	{
//		vi = vp[filename];
//		*ves = vi.Vshader;
//		*input = vi.InputL;
//	}
//
//	return hr;
//}
//
//void ResourceManager::Release()
//{
//	mp.clear();
//	vp.clear();
//	pp.clear();
//}
