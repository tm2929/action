#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
using namespace DirectX;

class Shader
{
public:
	Shader() {}
	Shader(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& immediateContext);
	bool Create(ID3D11Device* device, ID3D11DeviceContext* device_context, char * vsName,  char* psName, UINT numElements, D3D11_INPUT_ELEMENT_DESC* input_elemen);
	virtual ~Shader() {}

	bool CreateSprite(ID3D11Device* device);
	bool CreateSpriteBatch(ID3D11Device* device);
	bool CreateBoard(ID3D11Device* device);
	bool CreateGeometric(ID3D11Device* device);
	bool CreateModel(ID3D11Device* device);
	bool CreateMesh(ID3D11Device* device);

	bool CreateObj(ID3D11Device* device);
	bool CreateTexGeometric(ID3D11Device* device);

	void Activate(ID3D11DeviceContext* context);
	void DeActivate(ID3D11DeviceContext* context);
protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		VS = nullptr; // ���_�V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		PS = nullptr; // �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	GS = nullptr; // �W�I���g���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11HullShader>		HS = nullptr; //�@�n���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11DomainShader>		DS = nullptr;//�@�h���C���l�[���V�F�[�_


	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>		VertexLayout;
};


//----------------------------------------------
//
//	���_�f�[�^�\���̒�`
//
//----------------------------------------------

struct VERTEX
{
	XMFLOAT3 Pos;	//�ʒu
	XMFLOAT3 Normal;//�@��
	XMFLOAT2 Tex;	//UV���W
	XMFLOAT4 Color;	//���_�F
};