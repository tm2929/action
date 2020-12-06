
#include "ObjMesh.h"
#include "locale.h"
#include <string>


OBJMesh::OBJMesh(ID3D11Device* device,const char* FileName)
{
	HRESULT hr = LoadStaticMesh(device,FileName);
	assert(SUCCEEDED(hr));
}

//
//
//
OBJMesh::~OBJMesh()
{

	for (DWORD i = 0; i < m_dwNumMaterial; i++)
	{
		m_ppIndexBuffer[i]->Release();
	}

}

HRESULT OBJMesh::LoadMaterialFromFile(ID3D11Device* device,LPSTR FileName, MY_MATERIAL** ppMaterial)
{
	//マテリアルファイルを開いて内容を読み込む
	FILE* fp = NULL;
	fopen_s(&fp, FileName, "rt");
	char key[110] = { 0 };
	XMFLOAT4 v(0, 0, 0, 1);

	//マテリアル数を調べる
	m_dwNumMaterial = 0;
	while (!feof(fp))
	{
		//キーワード読み込み
		fscanf_s(fp, "%s ", key, sizeof(key));
		//マテリアル名
		if (strcmp(key, "newmtl") == 0)
		{
			m_dwNumMaterial++;
		}
	}
	MY_MATERIAL* pMaterial = new MY_MATERIAL[m_dwNumMaterial];

	//本読み込み	
	fseek(fp, SEEK_SET, 0);
	INT iMCount = -1;

	while (!feof(fp))
	{
		//キーワード読み込み
		fscanf_s(fp, "%s ", key, sizeof(key));
		//マテリアル名
		if (strcmp(key, "newmtl") == 0)
		{
			iMCount++;
			fscanf_s(fp, "%s ", key, sizeof(key));
			strcpy_s(pMaterial[iMCount].szName, key);
		}
		//テクスチャ名
		if (strcmp(key, "map_Kd") == 0)
		{
			char fullpass[255] = { 0 };
			fscanf_s(fp, "%s", &pMaterial[iMCount].szTextureName, sizeof(pMaterial[iMCount].szTextureName));
			strcpy_s(fullpass, filepass);
			strcat_s(fullpass, pMaterial[iMCount].szTextureName);

			//テクスチャーを作成
			pMaterial[iMCount].tex = std::make_unique<Texture>();
			//マルチバイト文字列をワイド文字列に変換
			WCHAR wfullpass[255] = { 0 };
			size_t wLen = 0;
			errno_t err = 0;
			setlocale(LC_ALL, "japanese");
			err = mbstowcs_s(&wLen, wfullpass, 255, fullpass, _TRUNCATE);
			pMaterial[iMCount].tex->Load(device,wfullpass);

			memset(wfullpass, 0, sizeof(wfullpass));

		}

		//Bumpテクスチャ名
		if (strcmp(key, "map_Bump") == 0)
		{
			char fullpass[255] = { 0 };
			fscanf_s(fp, "%s", &pMaterial[iMCount].szBumpTextureName, sizeof(pMaterial[iMCount].szBumpTextureName));
			strcpy_s(fullpass, filepass);
			strcat_s(fullpass, pMaterial[iMCount].szBumpTextureName);

			//テクスチャーを作成
			pMaterial[iMCount].ntex = std::make_unique<Texture>();
			//マルチバイト文字列をワイド文字列に変換
			WCHAR wfullpass[255] = { 0 };
			size_t wLen = 0;
			errno_t err = 0;
			setlocale(LC_ALL, "japanese");
			err = mbstowcs_s(&wLen, wfullpass, 255, fullpass, _TRUNCATE);
			pMaterial[iMCount].ntex->Load(device,wfullpass);
			memset(wfullpass, 0, sizeof(wfullpass));

		}

	}
	fclose(fp);

	*ppMaterial = pMaterial;
	return S_OK;
}
//
//
//
HRESULT OBJMesh::LoadStaticMesh(ID3D11Device* device,const char* FileName)
{
	
	float x, y, z;
	int v1 = 0, v2 = 0, v3 = 0;
	int vn1 = 0, vn2 = 0, vn3 = 0;
	int vt1 = 0, vt2 = 0, vt3 = 0;
	DWORD dwVCount = 0;//読み込みカウンター
	DWORD dwVNCount = 0;//読み込みカウンター
	DWORD dwVTCount = 0;//読み込みカウンター
	DWORD dwFCount = 0;//読み込みカウンター

	//char filepass[200] = { 0 };
	char key[200] = { 0 };
	//OBJファイルを開いて内容を読み込む
	FILE* fp = NULL;
	fopen_s(&fp, FileName, "rt");
	int len = strlen(FileName);
	bool flg = false;
	for (int i = len - 1; i >= 0; i--) {
		if (FileName[i] == '/')flg = true;
		if (FileName[i] == '\\')flg = true;
		if (flg) {
			filepass[i] = FileName[i];
		}
		else {
			filepass[i] = '\0';
		}
	}

	//事前に頂点数、ポリゴン数を調べる
	while (!feof(fp))
	{
		//キーワード読み込み
		fscanf_s(fp, "%s ", key, sizeof(key));
		//マテリアル読み込み
		if (strcmp(key, "mtllib") == 0)
		{
			fscanf_s(fp, "%s\0", key, sizeof(key));
			char fullpass[255] = { 0 };
			strcpy_s(fullpass, filepass);
			strcat_s(fullpass, key);
			LoadMaterialFromFile(device , fullpass, &m_pMaterial);

		}
		//頂点
		if (strcmp(key, "v") == 0)
		{
			m_dwNumVert++;
		}
		//法線
		if (strcmp(key, "vn") == 0)
		{
			dwVNCount++;
		}
		//テクスチャー座標
		if (strcmp(key, "vt") == 0)
		{
			dwVTCount++;
		}
		//フェイス（ポリゴン）
		if (strcmp(key, "f") == 0)
		{
			m_dwNumFace++;
		}
	}

	//一時的なメモリ確保（頂点バッファとインデックスバッファ）
	std::unique_ptr<MY_VERTEX[]> pvVertexBuffer = std::make_unique<MY_VERTEX[]>(m_dwNumFace * 3);
	std::unique_ptr<XMFLOAT3[]> pvCoord = std::make_unique<XMFLOAT3[]>(m_dwNumVert);
	std::unique_ptr<XMFLOAT3[]> pvNormal = std::make_unique<XMFLOAT3[]>(dwVNCount);
	std::unique_ptr<XMFLOAT2[]> pvTexture = std::make_unique<XMFLOAT2[]>(dwVTCount);


	//本読み込み	
	fseek(fp, SEEK_SET, 0);
	dwVCount = 0;
	dwVNCount = 0;
	dwVTCount = 0;
	dwFCount = 0;

	while (!feof(fp))
	{
		//キーワード 読み込み
		ZeroMemory(key, sizeof(key));
		fscanf_s(fp, "%s ", key, sizeof(key));

		//頂点 読み込み
		if (strcmp(key, "v") == 0)
		{
			fscanf_s(fp, "%f %f %f", &x, &y, &z);
			pvCoord[dwVCount].x = -x;
			pvCoord[dwVCount].y = y;
			pvCoord[dwVCount].z = z;
			dwVCount++;
		}

		//法線 読み込み
		if (strcmp(key, "vn") == 0)
		{
			fscanf_s(fp, "%f %f %f", &x, &y, &z);
			pvNormal[dwVNCount].x = -x;
			pvNormal[dwVNCount].y = y;
			pvNormal[dwVNCount].z = z;
			dwVNCount++;
		}

		//テクスチャー座標 読み込み
		if (strcmp(key, "vt") == 0)
		{
			fscanf_s(fp, "%f %f", &x, &y);
			pvTexture[dwVTCount].x = x;
			pvTexture[dwVTCount].y = 1 - y;//OBJファイルはY成分が逆なので合わせる
			dwVTCount++;
		}
	}

	//マテリアルの数だけインデックスバッファーを作成
	m_ppIndexBuffer = new ID3D11Buffer * [m_dwNumMaterial];

	//フェイス　読み込み　バラバラに収録されている可能性があるので、マテリアル名を頼りにつなぎ合わせる
	bool boFlag = false;
	int* piFaceBuffer = new int[m_dwNumFace * 3];//３頂点ポリゴンなので、1フェイス=3頂点(3インデックス)
	dwFCount = 0;
	DWORD dwPartFCount = 0;
	for (DWORD i = 0; i < m_dwNumMaterial; i++)
	{
		dwPartFCount = 0;
		fseek(fp, SEEK_SET, 0);

		while (!feof(fp))
		{
			//キーワード 読み込み
			ZeroMemory(key, sizeof(key));
			fscanf_s(fp, "%s ", key, sizeof(key));

			//フェイス 読み込み→頂点インデックスに
			if (strcmp(key, "usemtl") == 0)
			{
				fscanf_s(fp, "%s ", key, sizeof(key));
				if (strcmp(key, m_pMaterial[i].szName) == 0)
				{
					boFlag = true;
				}
				else
				{
					boFlag = false;
				}
			}
			if (strcmp(key, "f") == 0 && boFlag == true)
			{
				if (m_pMaterial[i].tex != NULL)//テクスチャーありサーフェイス
				{
					fscanf_s(fp, "%d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
				}
				else//テクスチャー無しサーフェイス
				{
					fscanf_s(fp, "%d//%d %d//%d %d//%d", &v1, &vn1, &v2, &vn2, &v3, &vn3);
				}

				//インデックスバッファー(面の向き変更)
				piFaceBuffer[dwPartFCount * 3] = dwFCount * 3;
				piFaceBuffer[dwPartFCount * 3 + 1] = dwFCount * 3 + 2;
				piFaceBuffer[dwPartFCount * 3 + 2] = dwFCount * 3 + 1;
				//頂点構造体に代入
				pvVertexBuffer[dwFCount * 3].vPos = pvCoord[v1 - 1];
				pvVertexBuffer[dwFCount * 3].vNorm = pvNormal[vn1 - 1];
				pvVertexBuffer[dwFCount * 3].vTex = pvTexture[vt1 - 1];
				pvVertexBuffer[dwFCount * 3 + 1].vPos = pvCoord[v2 - 1];
				pvVertexBuffer[dwFCount * 3 + 1].vNorm = pvNormal[vn2 - 1];
				pvVertexBuffer[dwFCount * 3 + 1].vTex = pvTexture[vt2 - 1];
				pvVertexBuffer[dwFCount * 3 + 2].vPos = pvCoord[v3 - 1];
				pvVertexBuffer[dwFCount * 3 + 2].vNorm = pvNormal[vn3 - 1];
				pvVertexBuffer[dwFCount * 3 + 2].vTex = pvTexture[vt3 - 1];

				dwPartFCount++;
				dwFCount++;
			}
		}
		if (dwPartFCount == 0)//使用されていないマテリアル対策
		{
			m_ppIndexBuffer[i] = NULL;
			continue;
		}

		//インデックスバッファーを作成
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(int) * dwPartFCount * 3;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = piFaceBuffer;
		HRESULT hr = device->CreateBuffer(&bd, &InitData, &m_ppIndexBuffer[i]);
		assert(SUCCEEDED(hr));

		m_pMaterial[i].dwNumFace = dwPartFCount;
	}
	delete[] piFaceBuffer;
	fclose(fp);

	HRESULT hr = S_OK;
	//バーテックスバッファーを作成
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(MY_VERTEX) * m_dwNumFace * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pvVertexBuffer.get();
	hr = device->CreateBuffer(&bd, &InitData, m_pVertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	//コンスタントバッファー作成　変換行列渡し用
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SIMPLECONSTANT_BUFFER0);
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DEFAULT;

	hr = device->CreateBuffer(&cb, NULL, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	return S_OK;
}



void OBJMesh::Update()
{
	//拡大
	XMMATRIX s = XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転
	XMMATRIX a = XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//移動
	XMMATRIX p = XMMatrixTranslation(pos.x, pos.y, pos.z);
	// 行列の合成
	XMMATRIX mW = s * a * p;
	XMStoreFloat4x4(&WorldMatrix, mW);

}

//
//
//
void OBJMesh::Render(ID3D11DeviceContext* deviceContext,Shader* shader, XMMATRIX* view, XMMATRIX* projection,
	D3D_PRIMITIVE_TOPOLOGY topology)
{
	// シェーダ設定
	shader->Activate(deviceContext);

	SIMPLECONSTANT_BUFFER0 cb;
	cb.world = WorldMatrix;
	XMStoreFloat4x4(&cb.matWVP, XMLoadFloat4x4(&WorldMatrix) * (*view) * (*projection));
	deviceContext->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	deviceContext->GSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	deviceContext->DSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());//★追加


	//プリミティブ・トポロジーをセット
	deviceContext->IASetPrimitiveTopology(topology);
	//バーテックスバッファーをセット
	UINT stride = sizeof(MY_VERTEX);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	//マテリアルの数だけ、それぞれのマテリアルのインデックスバッファ－を描画
	for (DWORD i = 0; i < m_dwNumMaterial; i++)
	{
		//使用されていないマテリアル対策
		if (m_pMaterial[i].dwNumFace == 0)
		{
			continue;
		}
		//インデックスバッファーをセット
		stride = sizeof(int);
		offset = 0;
		deviceContext->IASetIndexBuffer(m_ppIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);
		//テクスチャーをシェーダーに渡す

		if (m_pMaterial[i].szTextureName[0] != NULL)
		{
			m_pMaterial[i].tex->Set(deviceContext,0);
		}
		if (m_pMaterial[i].szBumpTextureName[0] != NULL)
		{
			m_pMaterial[i].ntex->Set(deviceContext,8);//ノーマルマップ
		}

		//プリミティブをレンダリング
		deviceContext->DrawIndexed(m_pMaterial[i].dwNumFace * 3, 0, 0);

		m_pMaterial[i].tex->Set(deviceContext,0, FALSE);

		m_pMaterial[i].ntex->Set(deviceContext,8, FALSE);



	}
	//シェーダー無効可
	shader->DeActivate(deviceContext);

}