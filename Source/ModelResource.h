#pragma once

#include <string>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fbxsdk.h>
class ModelResource
{
public:
	ModelResource(ID3D11Device* device, const char* filename, const char* ignoreRootMotionNodeName = nullptr);
	~ModelResource() {}

	struct Node
	{
		std::string			name;
		std::string			path;
		int					parentIndex;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Material
	{
		std::string			textureFilename;
		DirectX::XMFLOAT4	color = { 0.8f, 0.8f, 0.8f, 1.0f };

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Subset
	{
		UINT		startIndex = 0;
		UINT		indexCount = 0;
		int			materialIndex = 0;

		Material* material = nullptr;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT3	normal;
		DirectX::XMFLOAT2	texcoord;
		DirectX::XMFLOAT4	boneWeight;
		DirectX::XMUINT4	boneIndex;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};
	struct Face
	{
		DirectX::XMFLOAT3 position[3];
		int materialIndex;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};
	struct Mesh
	{
		std::vector<Vertex>						vertices;
		std::vector<UINT>							indices;
		std::vector<Subset>						subsets;
		std::vector<Face>							faces;
		int													nodeIndex;
		std::vector<int>								nodeIndices;
		std::vector<DirectX::XMFLOAT4X4>		offsetTransforms;

		Microsoft::WRL::ComPtr<ID3D11Buffer>	vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	indexBuffer;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct NodeKeyData
	{
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	nodeKeys;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};
	struct Animation
	{
		std::string					name;
		float						secondsLength;
		std::vector<Keyframe>		keyframes;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	// 各種データ取得
	const std::vector<Mesh>& GetMeshes() const { return meshes; }
	const std::vector<Node>& GetNodes() const { return nodes; }
	const std::vector<Animation>& GetAnimations() const { return animations; }

private:
	// ノードデータを構築
	void LoadNodes(FbxNode* fbxNode, int parentNodeIndex);
	void LoadNode(FbxNode* fbxNode, int parentNodeIndex);

	// メッシュデータを読み込み
	void LoadMeshes(ID3D11Device* device, FbxNode* fbxNode);
	void LoadMesh(ID3D11Device* device, FbxNode* fbxNode, FbxMesh* fbxMesh);

	// マテリアルデータを読み込み
	void LoadMaterials(ID3D11Device* device, const char* dirname, FbxScene* fbxScene);
	void LoadMaterial(ID3D11Device* device, const char* dirname, FbxSurfaceMaterial* fbxSurfaceMaterial);

	// アニメーションデータを読み込み
	void LoadAnimations(FbxScene* fbxScene, const char* name);

	// ノードパス取得
	std::string GetNodePath(FbxNode* fbxNode) const;

	// インデックスの検索
	int FindNodeIndex(const char* name);
	int FindMaterialIndex(FbxScene* fbxScene, const FbxSurfaceMaterial* fbxSurfaceMaterial);
	// モデルセットアップ
	void BuildModel(ID3D11Device* device, const char* dirname, const char* ignoreRootMotionNodeName);

private:
	std::vector<Node>		nodes;
	std::vector<Material>	materials;
	std::vector<Mesh>		meshes;
	std::vector<Animation>	animations;
	int						rootMotionNodeIndex = -1;
};

