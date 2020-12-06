#pragma once
#include<d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include<vector>
//‰½ƒtƒŒ[ƒ€•ª•Û‘¶‚·‚é‚©
#define FREAMECOUNT  5
#define VERTEXNUM  FREAMECOUNT*2
//#define USECOUNT  (FREAMECOUNT+(FREAMECOUNT-2))
class Trajectory
{
public:
	Trajectory(ID3D11Device* device, const wchar_t* file_name);
	void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4 viewProjection);
	void SwapPosition();
	void SetStartPosition(const DirectX::XMFLOAT3 startPosition) { this->startPosition = startPosition; }
	void SetEndPosition(const DirectX::XMFLOAT3 endPosition) { this->endPosition = endPosition; }
	DirectX::XMFLOAT3 color = { 1.f,0.5f,0.1f };
private:
	
	float w;
	struct Data
	{
		DirectX::XMFLOAT3 position;
		bool isFlag;
	};
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
	 std::vector<vertex> vertices;
	//vertex vertices[VERTEXNUM];
	DirectX::XMFLOAT3 position;

	struct cbuffer
	{
		DirectX::XMFLOAT4X4 view_projection;

	};
	DirectX::XMFLOAT3	startPosition = { 0,0,0 };
	DirectX::XMFLOAT3    endPosition = { 0,0,0 };

	Data 	trajectoryStart[FREAMECOUNT];
	Data 	trajectoryEnd[FREAMECOUNT];

	//DirectX::XMFLOAT3 catmullRomPosition[VERTEXNUM];
	//DirectX::XMFLOAT3 catmullRomStartPosition[FREAMECOUNT];
	//DirectX::XMFLOAT3 catmullRomEndPosition[FREAMECOUNT];
	std::vector<DirectX::XMFLOAT3> catmullRomStartPosition;
	std::vector<DirectX::XMFLOAT3> catmullRomEndPosition;
	int count;

	bool existFlag = false;

	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	D3D11_TEXTURE2D_DESC texture2d;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srview;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	 inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	nullConstantBuffer;

};
