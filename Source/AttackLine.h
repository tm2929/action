#pragma once
#include<d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
class AttackLine
{
public:
	static void Create(ID3D11Device* device)
	{
		if (attackLine != nullptr)return;
		attackLine = new AttackLine(device);
	}
	static AttackLine& GetInctance()
	{
		return *attackLine;
	}
	static void Destroy()
	{
		if (attackLine == nullptr)return;
		delete(attackLine);
		attackLine = nullptr;
	}
	AttackLine(ID3D11Device* device);
	void Update();
	void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4 viewProjection, const DirectX::XMFLOAT4& color);
	//setter
	//setter
	void SetPosition(const DirectX::XMFLOAT3& p) { position = p; }
	void SetScale(const DirectX::XMFLOAT3& s) { scale = s; }
	void SetAngle(const DirectX::XMFLOAT3& a) { angle = a; }
	void SetExistFlag(const bool e) { existFlag = e; }
	//getter
	const DirectX::XMFLOAT3& GetPosition()const { return position; }
	const DirectX::XMFLOAT3& GetScale()const { return scale; }
	const DirectX::XMFLOAT3& GetAngle()const { return angle; }
	const bool& GetExistFlag()const { return existFlag; }
private:
	static AttackLine* attackLine;
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;
		DirectX::XMFLOAT4 material_color;

	};
	DirectX::XMFLOAT3	position = { 0,0,0 };
	DirectX::XMFLOAT3	angle = { 0,0,0 };
	DirectX::XMFLOAT3	scale = { 1,1,1 };
	DirectX::XMMATRIX	world;
	bool existFlag = false;
	//	ID3D11VertexShader* vertexShader = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>	 inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	nullConstantBuffer;


};