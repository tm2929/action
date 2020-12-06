#include <stdlib.h>
#include <fstream>
#include <functional>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include"WICTextureLoader.h"
#include "Misc.h"
#include "Logger.h"
#include "ModelResource.h"
#include <string>
#include <Shlwapi.h>
#include "Encoding.h"
#include "Texture.h"
#pragma comment( lib, "Shlwapi.lib" )

// CEREALバージョン定義
CEREAL_CLASS_VERSION(ModelResource::Node, 1)
CEREAL_CLASS_VERSION(ModelResource::Material, 1)
CEREAL_CLASS_VERSION(ModelResource::Subset, 1)
CEREAL_CLASS_VERSION(ModelResource::Vertex, 1)
CEREAL_CLASS_VERSION(ModelResource::Mesh, 1)
CEREAL_CLASS_VERSION(ModelResource::NodeKeyData, 1)
CEREAL_CLASS_VERSION(ModelResource::Keyframe, 1)
CEREAL_CLASS_VERSION(ModelResource::Animation, 1)
CEREAL_CLASS_VERSION(ModelResource, 1)
// FbxDouble2 → XMFLOAT2
inline DirectX::XMFLOAT2 FbxDouble2ToFloat2(const FbxDouble2& fbxValue)
{
	return DirectX::XMFLOAT2(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1])
	);
}

// FbxDouble3 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble3ToFloat3(const FbxDouble3& fbxValue)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2])
	);
}

// FbxDouble4 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble4ToFloat3(const FbxDouble4& fbxValue)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2])
	);
}

// FbxDouble4 → XMFLOAT4
inline DirectX::XMFLOAT4 FbxDouble4ToFloat4(const FbxDouble4& fbxValue)
{
	return DirectX::XMFLOAT4(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2]),
		static_cast<float>(fbxValue[3])
	);
}

// FbxDouble4 → XMFLOAT4
inline DirectX::XMFLOAT4X4 FbxAMatrixToFloat4x4(const FbxAMatrix& fbxValue)
{
	return DirectX::XMFLOAT4X4(
		static_cast<float>(fbxValue[0][0]),
		static_cast<float>(fbxValue[0][1]),
		static_cast<float>(fbxValue[0][2]),
		static_cast<float>(fbxValue[0][3]),
		static_cast<float>(fbxValue[1][0]),
		static_cast<float>(fbxValue[1][1]),
		static_cast<float>(fbxValue[1][2]),
		static_cast<float>(fbxValue[1][3]),
		static_cast<float>(fbxValue[2][0]),
		static_cast<float>(fbxValue[2][1]),
		static_cast<float>(fbxValue[2][2]),
		static_cast<float>(fbxValue[2][3]),
		static_cast<float>(fbxValue[3][0]),
		static_cast<float>(fbxValue[3][1]),
		static_cast<float>(fbxValue[3][2]),
		static_cast<float>(fbxValue[3][3])
	);
}
// シリアライズ
namespace DirectX
{
	template<class Archive>
	void serialize(Archive& archive, XMUINT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}

template<class Archive>
void ModelResource::Node::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(name),
		CEREAL_NVP(path),
		CEREAL_NVP(parentIndex),
		CEREAL_NVP(scale),
		CEREAL_NVP(rotate),
		CEREAL_NVP(translate)
	);
}

template<class Archive>
void ModelResource::Material::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(textureFilename),
		CEREAL_NVP(color)
	);
}

template<class Archive>
void ModelResource::Subset::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(startIndex),
		CEREAL_NVP(indexCount),
		CEREAL_NVP(materialIndex)
	);
}

template<class Archive>
void ModelResource::Vertex::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(position),
		CEREAL_NVP(normal),
		CEREAL_NVP(texcoord),
		CEREAL_NVP(boneWeight),
		CEREAL_NVP(boneIndex)
	);
}

template<class Archive>
inline void ModelResource::Face::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(position[0]),
		CEREAL_NVP(position[1]),
		CEREAL_NVP(position[2]),
		CEREAL_NVP(materialIndex)

	);
}

template<class Archive>
void ModelResource::Mesh::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(vertices),
		CEREAL_NVP(indices),
		CEREAL_NVP(subsets),
		CEREAL_NVP(nodeIndex),
		CEREAL_NVP(nodeIndices),
		CEREAL_NVP(offsetTransforms)
	);
}

template<class Archive>
void ModelResource::NodeKeyData::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(scale),
		CEREAL_NVP(rotate),
		CEREAL_NVP(translate)
	);
}

template<class Archive>
void ModelResource::Keyframe::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(seconds),
		CEREAL_NVP(nodeKeys)
	);
}

template<class Archive>
void ModelResource::Animation::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(name),
		CEREAL_NVP(secondsLength),
		CEREAL_NVP(keyframes)
	);
}

// コンストラクタ
ModelResource::ModelResource(ID3D11Device* device, const char* filename, const char* ignoreRootMotionNodeName)
{
	// ディレクトリパス取得
	char drive[32], dir[256], dirname[256];
	::_splitpath_s(filename, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);

	::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);

	const char* ext = strrchr(filename, '.');
	if (::_stricmp(ext, ".fbx") == 0)
	{
		std::string name = filename;
		int path = name.find_last_of(".") + 1;
		std::string extension = "bin";
		std::string ddfname = name.substr(0, path);
		std::string path_name = ddfname + extension;
		if (PathFileExistsA(path_name.c_str()))
		{
			// デシリアライズ
			std::ifstream istream(path_name, std::ios::binary);
			if (istream.is_open())
			{
				cereal::BinaryInputArchive archive(istream);
				try
				{
					archive(
						CEREAL_NVP(nodes),
						CEREAL_NVP(materials),
						CEREAL_NVP(meshes),
						CEREAL_NVP(animations)
					);
				}
				catch (...)
				{
					LOGGER("model deserialize failed.\n%s\n", filename);
					return;
				}
			}
		}
		else
		{
			// FBXのファイルパスはUTF-8にする必要がある
			char fbxFilename[256];
			Encoding::StringToUTF8(filename, fbxFilename, sizeof(fbxFilename));

			FbxManager* fbxManager = FbxManager::Create();

			// FBXに対する入出力を定義する
			FbxIOSettings* fbxIOS = FbxIOSettings::Create(fbxManager, IOSROOT);	// 特別な理由がない限りIOSROOTを指定
			fbxManager->SetIOSettings(fbxIOS);

			// インポータを生成
			FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
			bool result = fbxImporter->Initialize(fbxFilename, -1, fbxManager->GetIOSettings());	// -1でファイルフォーマット自動判定
			_ASSERT_EXPR_A(result, "FbxImporter::Initialize() : Failed!!\n");

			// SceneオブジェクトにFBXファイル内の情報を流し込む
			FbxScene* fbxScene = FbxScene::Create(fbxManager, "scene");
			fbxImporter->Import(fbxScene);
			fbxImporter->Destroy();	// シーンを流し込んだらImporterは解放してOK

			// ジオメトリを三角形化しておく
			FbxGeometryConverter fbxGeometryConverter(fbxManager);
			fbxGeometryConverter.Triangulate(fbxScene, true);
			fbxGeometryConverter.RemoveBadPolygonsFromMeshes(fbxScene);
#if 0
			// DirectX座標系へ変換
			FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
			if (sceneAxisSystem != FbxAxisSystem::DirectX)
			{
				FbxAxisSystem::DirectX.ConvertScene(fbxScene);
			}
#elif 0
			// OpenGL座標系へ変換
			FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
			if (sceneAxisSystem != FbxAxisSystem::OpenGL)
			{
				FbxAxisSystem::OpenGL.ConvertScene(fbxScene);
			}
#endif
			// モデル読み込み
			std::vector<FbxNode*> fbxNodes;
			FbxNode* fbxRootNode = fbxScene->GetRootNode();
			LoadMaterials(device, dirname, fbxScene);
			LoadNodes(fbxRootNode, -1);
			LoadMeshes(device, fbxRootNode);

			// アニメーション読み込み
			char name[256];
			::_splitpath_s(filename, nullptr, 0, nullptr, 0, name, 256, nullptr, 0);

			LoadAnimations(fbxScene, name);

			// マネージャ解放
			fbxManager->Destroy();		// 関連するすべてのオブジェクトが解放される

			std::ofstream ofs(path_name, std::ios::binary);
			if (ofs.is_open())
			{
				cereal::BinaryOutputArchive archive(ofs);
				try
				{
					archive(
						CEREAL_NVP(nodes),
						CEREAL_NVP(materials),
						CEREAL_NVP(meshes),
						CEREAL_NVP(animations)
					);
				}
				catch (...)
				{
					LOGGER("model deserialize failed.\n%s\n", filename);
					return;
				}
			}
		}
	}
	else
	{
		// デシリアライズ
		std::ifstream istream(filename, std::ios::binary);
		if (istream.is_open())
		{
			cereal::BinaryInputArchive archive(istream);
			try
			{
				archive(
					CEREAL_NVP(nodes),
					CEREAL_NVP(materials),
					CEREAL_NVP(meshes),
					CEREAL_NVP(animations)
				);
			}
			catch (...)
			{
				LOGGER("model deserialize failed.\n%s\n", filename);
				return;
			}
		}
	}
	// モデル構築
	BuildModel(device, dirname, ignoreRootMotionNodeName);
}

// モデル構築
void ModelResource::BuildModel(ID3D11Device* device, const char* dirname, const char* ignoreRootMotionNodeName)
{

	for (Material& material : materials)
	{
		errno_t err;
		// 相対パスの解決
		const char* hullFilename = material.textureFilename.c_str();

		char filename[256], textureFile[256] = { 0 }, extension[256];

		err = ::_splitpath_s(hullFilename, nullptr, 0, nullptr, 0,
			filename, sizeof(filename) / sizeof(filename[0]), extension, sizeof(extension) / sizeof(extension[0]));
		if (err != 0)
		{
			assert(!"Not File");
		}
		sprintf_s(textureFile, "%s%s", filename, extension);

		//合成
		char FilePath[256];
		sprintf_s(FilePath, "%s%s", dirname, textureFile);

		// マルチバイト文字からワイド文字へ変換
		wchar_t wfilename[256];
		::MultiByteToWideChar(CP_ACP, 0, FilePath, -1, wfilename, 256);

		// テクスチャ読み込み
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		D3D11_TEXTURE2D_DESC desc = { 0 };
		HRESULT hr = load_texture_from_file(device, wfilename, material.shaderResourceView.GetAddressOf(),&desc); /*DirectX::CreateWICTextureFromFile(device, wfilename, resource.GetAddressOf(), material.shaderResourceView.GetAddressOf());*/
		if (FAILED(hr))
		{
			LOGGER("load failed : %s\n", filename);

			// 読み込み失敗したらダミーテクスチャを作る
			const int width = 8;
			const int height = 8;
			UINT pixels[width * height];
			::memset(pixels, 0xFF, sizeof(pixels));
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			D3D11_SUBRESOURCE_DATA data;
			::memset(&data, 0, sizeof(data));
			data.pSysMem = pixels;
			data.SysMemPitch = width;

			Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
			HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			hr = device->CreateShaderResourceView(texture.Get(), nullptr, material.shaderResourceView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
	}

	for (Mesh& mesh : meshes)
	{
		// サブセット
		for (Subset& subset : mesh.subsets)
		{
			subset.material = &materials.at(subset.materialIndex);
		}

		// 頂点バッファ
		{
			D3D11_BUFFER_DESC bufferDesc = {};
			D3D11_SUBRESOURCE_DATA subresourceData = {};

			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
			//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			subresourceData.pSysMem = mesh.vertices.data();
			subresourceData.SysMemPitch = 0;
			subresourceData.SysMemSlicePitch = 0;

			HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// インデックスバッファ
		{
			D3D11_BUFFER_DESC bufferDesc = {};
			D3D11_SUBRESOURCE_DATA subresourceData = {};

			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * mesh.indices.size());
			//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			subresourceData.pSysMem = mesh.indices.data();
			subresourceData.SysMemPitch = 0; //Not use for index buffers.
			subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
			HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
	}
	// 無視するルートモーションを検索
	if (ignoreRootMotionNodeName != nullptr)
	{
		rootMotionNodeIndex = -1;
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			if (nodes.at(i).name == ignoreRootMotionNodeName)
			{
				rootMotionNodeIndex = static_cast<int>(i);
				break;
			}
		}
	}
}


// FBXノードを再帰的に辿ってデータを構築する
void ModelResource::LoadNodes(FbxNode* fbxNode, int parentNodeIndex)
{
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();
	FbxNodeAttribute::EType fbxNodeAttributeType = FbxNodeAttribute::EType::eUnknown;
	if (fbxNodeAttribute != nullptr)
	{
		fbxNodeAttributeType = fbxNodeAttribute->GetAttributeType();
	}

	switch (fbxNodeAttributeType)
	{
	case FbxNodeAttribute::eUnknown:
	case FbxNodeAttribute::eNull:
	case FbxNodeAttribute::eMarker:
	case FbxNodeAttribute::eMesh:
	case FbxNodeAttribute::eSkeleton:
		LoadNode(fbxNode, parentNodeIndex);
		break;
	}
	// 再帰的に子ノードを処理する
	parentNodeIndex = static_cast<int>(nodes.size() - 1);
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		LoadNodes(fbxNode->GetChild(i), parentNodeIndex);
	}
}

void ModelResource::LoadNode(FbxNode* fbxNode, int parentNodeIndex)
{
	FbxAMatrix& fbxLocalTransform = fbxNode->EvaluateLocalTransform();

	Node node;
	node.name = fbxNode->GetName();
	node.path = GetNodePath(fbxNode);
	node.parentIndex = parentNodeIndex;
	node.scale = FbxDouble4ToFloat3(fbxLocalTransform.GetS());
	node.rotate = FbxDouble4ToFloat4(fbxLocalTransform.GetQ());
	node.translate = FbxDouble4ToFloat3(fbxLocalTransform.GetT());

	nodes.emplace_back(node);
}

void ModelResource::LoadMeshes(ID3D11Device* device, FbxNode* fbxNode)
{
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();
	FbxNodeAttribute::EType fbxNodeAttributeType = FbxNodeAttribute::EType::eUnknown;

	if (fbxNodeAttribute != nullptr)
	{
		fbxNodeAttributeType = fbxNodeAttribute->GetAttributeType();
	}

	switch (fbxNodeAttributeType)
	{
	case FbxNodeAttribute::eMesh:
		LoadMesh(device, fbxNode, static_cast<FbxMesh*>(fbxNodeAttribute));
		break;
	}
	// 再帰的に子ノードを処理する
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		LoadMeshes(device, fbxNode->GetChild(i));
	}
}

void ModelResource::LoadMesh(ID3D11Device* device, FbxNode* fbxNode, FbxMesh* fbxMesh)
{
	int fbxControlPointsCount = fbxMesh->GetControlPointsCount();
	//int fbxPolygonVertexCount = fbxMesh->GetPolygonVertexCount();
	//const int* fbxPolygonVertices = fbxMesh->GetPolygonVertices();

	int fbxMaterialCount = fbxNode->GetMaterialCount();
	int fbxPolygonCount = fbxMesh->GetPolygonCount();
	std::string& fbxNodePath = GetNodePath(fbxNode);

	meshes.emplace_back(Mesh());
	Mesh& mesh = meshes.back();
	mesh.subsets.resize(fbxMaterialCount > 0 ? fbxMaterialCount : 1);
	mesh.nodeIndex = FindNodeIndex(fbxNodePath.c_str());

	// サブセットのマテリアル設定
	for (int fbxMaterialIndex = 0; fbxMaterialIndex < fbxMaterialCount; ++fbxMaterialIndex)
	{
		const FbxSurfaceMaterial* fbxSurfaceMaterial = fbxNode->GetMaterial(fbxMaterialIndex);

		Subset& subset = mesh.subsets.at(fbxMaterialIndex);
		subset.materialIndex = FindMaterialIndex(fbxNode->GetScene(), fbxSurfaceMaterial);
	}

	// サブセットの頂点インデックス範囲設定
	if (fbxMaterialCount > 0)
	{
		for (int fbxPolygonIndex = 0; fbxPolygonIndex < fbxPolygonCount; ++fbxPolygonIndex)
		{
			int fbxMaterialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(fbxPolygonIndex);
			mesh.subsets.at(fbxMaterialIndex).indexCount += 3;
		}

		int offset = 0;
		for (Subset& subset : mesh.subsets)
		{
			subset.startIndex = offset;
			offset += subset.indexCount;

			subset.indexCount = 0;
		}
	}

	// 頂点影響力データ
	struct BoneInfluence
	{
		int useCount = 0;
		int indices[4] = { 0, 0, 0, 0 };
		float weights[4] = { 1.0f, 0.0f, 0.0f, 0.0f };

		void Add(int index, float weight)
		{
			if (useCount < 4)
			{
				indices[useCount] = index;
				weights[useCount] = weight;
				useCount++;
			}
		}
	};
	// 頂点影響力データを抽出する
	std::vector<BoneInfluence> boneInfluences;
	{
		boneInfluences.resize(fbxControlPointsCount);

		// スキニングに必要な情報を取得する
		int fbxDeformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int fbxDeformerIndex = 0; fbxDeformerIndex < fbxDeformerCount; ++fbxDeformerIndex)
		{
			FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(fbxDeformerIndex, FbxDeformer::eSkin));

			int fbxClusterCount = fbxSkin->GetClusterCount();
			for (int fbxClusterIndex = 0; fbxClusterIndex < fbxClusterCount; ++fbxClusterIndex)
			{
				FbxCluster* fbxCluster = fbxSkin->GetCluster(fbxClusterIndex);

				// 頂点影響力データを抽出する
				{
					int fbxClusterControlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
					const int* fbxControlPointIndices = fbxCluster->GetControlPointIndices();
					const double* fbxControlPointWeights = fbxCluster->GetControlPointWeights();

					for (int i = 0; i < fbxClusterControlPointIndicesCount; ++i)
					{
						BoneInfluence& boneInfluence = boneInfluences.at(fbxControlPointIndices[i]);

						boneInfluence.Add(fbxClusterIndex, static_cast<float>(fbxControlPointWeights[i]));
					}
				}

				// ボーン変換行列用の逆行列の計算をする
				{
					// メッシュ空間からワールド空間への変換行列
					FbxAMatrix fbxMeshSpaceTransform;
					fbxCluster->GetTransformMatrix(fbxMeshSpaceTransform);

					// ボーン空間からワールド空間への変換行列
					FbxAMatrix fbxBoneSpaceTransform;
					fbxCluster->GetTransformLinkMatrix(fbxBoneSpaceTransform);

					// ボーン逆行列を計算する
					FbxAMatrix fbxInverseTransform = fbxBoneSpaceTransform.Inverse() * fbxMeshSpaceTransform;

					DirectX::XMFLOAT4X4 offsetTransform = FbxAMatrixToFloat4x4(fbxInverseTransform);
					mesh.offsetTransforms.emplace_back(offsetTransform);

					std::string& fbxNodePath = GetNodePath(fbxCluster->GetLink());
					int nodeIndex = FindNodeIndex(fbxNodePath.c_str());
					mesh.nodeIndices.emplace_back(nodeIndex);
				}
			}
		}
	}

	// ジオメトリ行列
	FbxAMatrix fbxGeometricTransform(
		fbxNode->GetGeometricTranslation(FbxNode::eSourcePivot),
		fbxNode->GetGeometricRotation(FbxNode::eSourcePivot),
		fbxNode->GetGeometricScaling(FbxNode::eSourcePivot)
	);
	DirectX::XMFLOAT4X4 geometricTransform = FbxAMatrixToFloat4x4(fbxGeometricTransform);
	DirectX::XMMATRIX GM = DirectX::XMLoadFloat4x4(&geometricTransform);

	// UVセット名
	FbxStringList fbxUVSetNames;
	fbxMesh->GetUVSetNames(fbxUVSetNames);

	// 頂点データ
	mesh.vertices.resize(fbxPolygonCount * 3);
	mesh.indices.resize(fbxPolygonCount * 3);

	int vertexCount = 0;
	const FbxVector4* fbxControlPoints = fbxMesh->GetControlPoints();

	Face face;
	for (int fbxPolygonIndex = 0; fbxPolygonIndex < fbxPolygonCount; ++fbxPolygonIndex)
	{
		// ポリゴンに適用されているマテリアルインデックスを取得する
		int fbxMaterialIndex = 0;
		if (fbxMaterialCount > 0)
		{
			fbxMaterialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(fbxPolygonIndex);
		}

		Subset& subset = mesh.subsets.at(fbxMaterialIndex);
		const int indexOffset = subset.startIndex + subset.indexCount;
		face.materialIndex = fbxMaterialIndex;
		for (int fbxVertexIndex = 0; fbxVertexIndex < 3; ++fbxVertexIndex)
		{
			Vertex vertex;

			int fbxControlPointIndex = fbxMesh->GetPolygonVertex(fbxPolygonIndex, fbxVertexIndex);
			// Position
			{
				DirectX::XMFLOAT3 position = FbxDouble4ToFloat3(fbxControlPoints[fbxControlPointIndex]);

				DirectX::XMVECTOR V = DirectX::XMLoadFloat3(&position);
				V = DirectX::XMVector3TransformCoord(V, GM);
				DirectX::XMStoreFloat3(&vertex.position, V);

				DirectX::XMStoreFloat3(&face.position[fbxVertexIndex], V);
			}

			// Weight
			{
				BoneInfluence& boneInfluence = boneInfluences.at(fbxControlPointIndex);
				vertex.boneIndex.x = boneInfluence.indices[0];
				vertex.boneIndex.y = boneInfluence.indices[1];
				vertex.boneIndex.z = boneInfluence.indices[2];
				vertex.boneIndex.w = boneInfluence.indices[3];
				vertex.boneWeight.x = boneInfluence.weights[0];
				vertex.boneWeight.y = boneInfluence.weights[1];
				vertex.boneWeight.z = boneInfluence.weights[2];
				vertex.boneWeight.w = 1.0f - (vertex.boneWeight.x + vertex.boneWeight.y + vertex.boneWeight.z);
				//vertex.boneWeight.w = boneInfluence.weights[3];
			}

			// Normal
			if (fbxMesh->GetElementNormalCount() > 0)
			{
				FbxVector4 fbxNormal;
				fbxMesh->GetPolygonVertexNormal(fbxPolygonIndex, fbxVertexIndex, fbxNormal);

				DirectX::XMFLOAT3 normal = FbxDouble4ToFloat3(fbxNormal);
				DirectX::XMVECTOR V = DirectX::XMLoadFloat3(&normal);
				V = DirectX::XMVector3TransformCoord(V, GM);
				V = DirectX::XMVector3Normalize(V);
				DirectX::XMStoreFloat3(&vertex.normal, V);

				vertex.normal = FbxDouble4ToFloat3(fbxNormal);
			}
			else
			{
				vertex.normal = DirectX::XMFLOAT3(0, 0, 0);
			}

			// Texcoord
			if (fbxMesh->GetElementUVCount() > 0)
			{
				bool fbxUnmappedUV;
				FbxVector2 fbxUV;
				fbxMesh->GetPolygonVertexUV(fbxPolygonIndex, fbxVertexIndex, fbxUVSetNames[0], fbxUV, fbxUnmappedUV);
				fbxUV[1] = 1.0 - fbxUV[1];
				vertex.texcoord = FbxDouble2ToFloat2(fbxUV);
			}
			else
			{
				vertex.texcoord = DirectX::XMFLOAT2(0, 0);
			}

			mesh.indices.at(indexOffset + fbxVertexIndex) = vertexCount;
			mesh.vertices.at(vertexCount) = vertex;
			vertexCount++;
		}
		mesh.faces.push_back(face);
		subset.indexCount += 3;
	}
}

// FBXシーン内のFBXマテリアルからマテリアルデータを読み込み
void ModelResource::LoadMaterials(ID3D11Device* device, const char* dirname, FbxScene* fbxScene)
{
	int fbxMaterialCount = fbxScene->GetMaterialCount();
	if (fbxMaterialCount > 0)
	{
		for (int fbxMaterialIndex = 0; fbxMaterialIndex < fbxMaterialCount; ++fbxMaterialIndex)
		{
			FbxSurfaceMaterial* fbxSurfaceMaterial = fbxScene->GetMaterial(fbxMaterialIndex);
			LoadMaterial(device, dirname, fbxSurfaceMaterial);
		}
	}
	else
	{
		Material material;
		material.color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		materials.emplace_back(material);
	}
}

// FBXマテリアルからマテリアルデータを読み込み
void ModelResource::LoadMaterial(ID3D11Device* device, const char* dirname, FbxSurfaceMaterial* fbxSurfaceMaterial)
{
	bool ret = false;

	Material material;

	// ディフューズカラー
	FbxProperty fbxDiffuseProperty = fbxSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	FbxProperty fbxDiffuseFactorProperty = fbxSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
	if (fbxDiffuseProperty.IsValid() && fbxDiffuseFactorProperty.IsValid())
	{
		FbxDouble fbxFactor = fbxDiffuseFactorProperty.Get<FbxDouble>();
		FbxDouble3 fbxColor = fbxDiffuseProperty.Get<FbxDouble3>();

		material.color.x = static_cast<float>(fbxColor[0] * fbxFactor);
		material.color.y = static_cast<float>(fbxColor[1] * fbxFactor);
		material.color.z = static_cast<float>(fbxColor[2] * fbxFactor);
		material.color.w = 1.0f;
	}
#if 0
	FbxProperty fbx_transparency_factor_property = fbxSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if (fbx_transparency_factor_property.IsValid())
	{
		FbxDouble fbxFactor = fbx_transparency_factor_property.Get<FbxDouble>();

		material.color.w = static_cast<float>(fbxFactor);
	}
#endif
	// ディフューズテクスチャ
	if (fbxDiffuseProperty.IsValid())
	{
		int fbxTextureCount = fbxDiffuseProperty.GetSrcObjectCount<FbxFileTexture>();
		if (fbxTextureCount > 0)
		{
			FbxFileTexture* fbxTexture = fbxDiffuseProperty.GetSrcObject<FbxFileTexture>();

			material.textureFilename = fbxTexture->GetRelativeFileName();
		}
	}

	materials.emplace_back(material);
}

// アニメーションデータを読み込み
void ModelResource::LoadAnimations(FbxScene* fbxScene, const char* name)
{
	// すべてのアニメーション名を取得
	FbxArray<FbxString*> fbxAnimStackNames;
	fbxScene->FillAnimStackNameArray(fbxAnimStackNames);

	int fbxAnimationCount = fbxAnimStackNames.Size();
	for (int fbxAnimationIndex = 0; fbxAnimationIndex < fbxAnimationCount; ++fbxAnimationIndex)
	{
		animations.emplace_back(Animation());
		Animation& animation = animations.back();

		// アニメーションデータのサンプリング設定
		FbxTime::EMode fbxTimeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		FbxTime fbxFrameTime;
		fbxFrameTime.SetTime(0, 0, 0, 1, 0, fbxTimeMode);

		float samplingRate = static_cast<float>(fbxFrameTime.GetFrameRate(fbxTimeMode));
		float samplingTime = 1.0f / samplingRate;

		FbxString* fbxAnimStackName = fbxAnimStackNames.GetAt(fbxAnimationIndex);
		FbxAnimStack* fbxAnimStack = fbxScene->FindMember<FbxAnimStack>(fbxAnimStackName->Buffer());

		// 再生するアニメーションを指定する。
		fbxScene->SetCurrentAnimationStack(fbxAnimStack);

		// アニメーションの再生開始時間と再生終了時間を取得する
		FbxTakeInfo* fbxTakeInfo = fbxScene->GetTakeInfo(fbxAnimStackName->Buffer());
		FbxTime fbxStartTime = fbxTakeInfo->mLocalTimeSpan.GetStart();
		FbxTime fbxEndTime = fbxTakeInfo->mLocalTimeSpan.GetStop();

		// 抽出するデータは60フレーム基準でサンプリングする
		FbxTime fbxSamplingStep;
		fbxSamplingStep.SetTime(0, 0, 1, 0, 0, fbxTimeMode);
		fbxSamplingStep = static_cast<FbxLongLong>(fbxSamplingStep.Get() * samplingTime);

		int startFrame = static_cast<int>(fbxStartTime.Get() / fbxSamplingStep.Get());
		int endFrame = static_cast<int>(fbxEndTime.Get() / fbxSamplingStep.Get());
		int frameCount = static_cast<int>((fbxEndTime.Get() - fbxStartTime.Get()) / fbxSamplingStep.Get());

		// アニメーションの対象となるノードを列挙する
		std::vector<FbxNode*> fbxNodes;
#if 0
		// ノード名を比較して対象ノードを列挙する
		// ※名前が重複していると失敗する場合がある
		FbxNode* fbxRootNode = fbxScene->GetRootNode();
		for (Node& node : nodes)
		{
			FbxNode* fbxNode = fbxRootNode->FindChild(node.name.c_str(), true, true);
			fbxNodes.emplace_back(fbxNode);
		}
#else
		// ノードの完全パスを比較して対象ノードを列挙する（重い）
		// ※必ずモデルとアニメーションのFBXのノードツリー構成が一致している必要がある
		for (Node& node : nodes)
		{
			FbxNode* fbxAnimationNode = nullptr;
			for (int fbxNodeIndex = 0; fbxNodeIndex < fbxScene->GetNodeCount(); ++fbxNodeIndex)
			{
				FbxNode* fbxNode = fbxScene->GetNode(fbxNodeIndex);
				std::string& nodePath = GetNodePath(fbxNode);
				if (node.path == nodePath)
				{
					fbxAnimationNode = fbxNode;
					break;
				}
			}
			fbxNodes.emplace_back(fbxAnimationNode);
		}
#endif
		// アニメーション名
		char animationName[256];
		::sprintf_s(animationName, "%s/%s", name, fbxAnimStackName->Buffer());
		animation.name = animationName;

		// アニメーションデータを抽出する
		animation.secondsLength = frameCount * samplingTime;
		animation.keyframes.resize(frameCount + 1);

		float seconds = 0.0f;
		Keyframe* keyframe = animation.keyframes.data();
		size_t fbxNodeCount = fbxNodes.size();
		FbxTime fbxCurrentTime = fbxStartTime;
		for (FbxTime fbxCurrentTime = fbxStartTime; fbxCurrentTime < fbxEndTime; fbxCurrentTime += fbxSamplingStep, ++keyframe)
		{
			// キーフレーム毎の姿勢データを取り出す。
			keyframe->seconds = seconds;
			keyframe->nodeKeys.resize(fbxNodeCount);
			for (size_t fbxNodeIndex = 0; fbxNodeIndex < fbxNodeCount; ++fbxNodeIndex)
			{
				NodeKeyData& keyData = keyframe->nodeKeys.at(fbxNodeIndex);
				FbxNode* fbxNode = fbxNodes.at(fbxNodeIndex);
				if (fbxNode == nullptr)
				{
					// アニメーション対象のノードがなかったのでダミーデータを設定
					Node& node = nodes.at(fbxNodeIndex);
					keyData.scale = node.scale;
					keyData.rotate = node.rotate;
					keyData.translate = node.translate;
				}
				else if (fbxNodeIndex == rootMotionNodeIndex)
				{
					// ルートモーションは無視する
					Node& node = nodes.at(fbxNodeIndex);
					keyData.scale = DirectX::XMFLOAT3(1, 1, 1);
					keyData.rotate = DirectX::XMFLOAT4(0, 0, 0, 1);
					keyData.translate = DirectX::XMFLOAT3(0, 0, 0);
				}
				else
				{
					// 指定時間のローカル行列からスケール値、回転値、移動値を取り出す。
					const FbxAMatrix& fbxLocalTransform = fbxNode->EvaluateLocalTransform(fbxCurrentTime);

					keyData.scale = FbxDouble4ToFloat3(fbxLocalTransform.GetS());
					keyData.rotate = FbxDouble4ToFloat4(fbxLocalTransform.GetQ());
					keyData.translate = FbxDouble4ToFloat3(fbxLocalTransform.GetT());
				}
			}
			seconds += samplingTime;
		}
	}

	// 後始末
	for (int i = 0; i < fbxAnimationCount; i++)
	{
		delete fbxAnimStackNames[i];
	}
}

// ノードパス取得
std::string ModelResource::GetNodePath(FbxNode* fbxNode) const
{
	std::string parentNodeName;

	FbxNode* fbxParentNode = fbxNode->GetParent();
	if (fbxParentNode != nullptr)
	{
		parentNodeName = GetNodePath(fbxParentNode);
		return parentNodeName + "/" + fbxNode->GetName();
	}

	return fbxNode->GetName();
}

// ノードインデックスを取得する
int ModelResource::FindNodeIndex(const char* nodePath)
{
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].path == nodePath)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

// マテリアルインデックスを取得する
int ModelResource::FindMaterialIndex(FbxScene* fbxScene, const FbxSurfaceMaterial* fbxSurfaceMaterial)
{
	int fbxMaterialCount = fbxScene->GetMaterialCount();

	for (int i = 0; i < fbxMaterialCount; ++i)
	{
		if (fbxScene->GetMaterial(i) == fbxSurfaceMaterial)
		{
			return i;
		}
	}
	return -1;
}