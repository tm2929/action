#pragma once
#include <stdio.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "Texture.h"
#include "Shader.h"

//--------------------------------------------------------------------
//	Mesh�N���X
//--------------------------------------------------------------------


//���_�̍\����
struct MY_VERTEX
{
	XMFLOAT3 vPos;
	XMFLOAT3 vNorm;
	XMFLOAT2 vTex;
};
struct SIMPLECONSTANT_BUFFER0
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 matWVP;


};

struct SIMPLECONSTANT_BUFFER1
{
	XMFLOAT4 vAmbient;//�A���r�G���g��
	XMFLOAT4 vDiffuse;//�f�B�t���[�Y�F
	XMFLOAT4 vSpecular;//���ʔ���
};
//�I���W�i���@�}�e���A���\����
struct MY_MATERIAL
{
	CHAR szName[110];
	CHAR szTextureName[110];//�e�N�X�`���[�t�@�C����
	CHAR szBumpTextureName[110];//Bump�e�N�X�`���[�t�@�C����

	std::unique_ptr<Texture> tex = nullptr;
	std::unique_ptr<Texture> ntex = nullptr;

	DWORD dwNumFace;//���̃}�e���A���ł���|���S����
	MY_MATERIAL()
	{
		ZeroMemory(this, sizeof(MY_MATERIAL));
	}
	~MY_MATERIAL()
	{
	}
};

//
//
//
class OBJMesh
{
private:
	DWORD m_dwNumVert = 0;
	DWORD m_dwNumFace = 0;
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ID3D11Buffer** m_ppIndexBuffer;

	ComPtr<ID3D11Buffer> ConstantBuffer;
	ComPtr<ID3D11Buffer> ConstantBufferPerMaterial;

	DWORD m_dwNumMaterial;
	MY_MATERIAL* m_pMaterial;

	char filepass[200] = { 0 };//OBJ/MTL�p�X��

	//std::unique_ptr<MY_MATERIAL>m_pMaterial;

	//���
	XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	XMFLOAT3 scale = { 5.0f, 5.0f, 5.0f };
	XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4X4 WorldMatrix;
	HRESULT LoadMaterialFromFile(ID3D11Device* device,LPSTR FileName, MY_MATERIAL** ppMaterial);
	HRESULT LoadStaticMesh(ID3D11Device* device,const char* FileName);

public:
	OBJMesh(ID3D11Device* device);
	OBJMesh(ID3D11Device* device,const char* FileName);
	//OBJ_Mesh(LPSTR FileName);
	~OBJMesh();
	//HRESULT Load(LPSTR FileName);
	void Render(ID3D11DeviceContext* deviceContext,Shader* shader, XMMATRIX* view, XMMATRIX* projection, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�s��X�V
	void Update();
	void SetPos(const XMFLOAT3& pos) { this->pos = pos; }
	void SetScale(const XMFLOAT3& scale) { this->scale = scale; }
	void SetAngle(const XMFLOAT3& angle) { this->angle = angle; }
	XMFLOAT3 GetPos() { return pos; }



};