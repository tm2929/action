//#pragma once
//
//#include <stdlib.h>
//#include <fstream>
//#include <functional>
//#include <cereal/cereal.hpp>
//#include <cereal/archives/binary.hpp>
//#include <cereal/types/string.hpp>
//#include <cereal/types/vector.hpp>
//
//#include <string>
//#include <vector>
//#include <wrl.h>
//#include <d3d11.h>
//#include <directxmath.h>
//
//
//struct ModelData
//{
//
//	ModelData() = default;
//	~ModelData() = default;
//	ModelData(const char* filename, std::string extension = "bin");
//	struct Node
//	{
//		std::string			name;
//		std::string			path;
//		int					parentIndex;
//		DirectX::XMFLOAT3	scale;
//		DirectX::XMFLOAT4	rotate;
//		DirectX::XMFLOAT3	translate;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	struct Material
//	{
//		std::string			textureFilename;
//		DirectX::XMFLOAT4	color = { 0.8f, 0.8f, 0.8f, 1.0f };
//
//		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	struct Subset
//	{
//		UINT		startIndex = 0;
//		UINT		indexCount = 0;
//		int			materialIndex = 0;
//
//		Material* material = nullptr;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	struct Vertex
//	{
//		DirectX::XMFLOAT3	position;
//		DirectX::XMFLOAT3	normal;
//		DirectX::XMFLOAT2	texcoord;
//		DirectX::XMFLOAT4	boneWeight;
//		DirectX::XMUINT4	 boneIndex;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	struct Mesh
//	{
//		std::vector<Vertex>						vertices;
//		std::vector<UINT>						indices;
//		std::vector<Subset>						subsets;
//
//		int										nodeIndex;
//		std::vector<int>						nodeIndices;
//		DirectX::XMFLOAT4X4 globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
//
//		std::vector<DirectX::XMFLOAT4X4>		inverseTransforms;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	struct NodeKeyData
//	{
//		DirectX::XMFLOAT3	scale;
//		DirectX::XMFLOAT4	rotate;
//		DirectX::XMFLOAT3	translate;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	struct Keyframe
//	{
//		float						seconds;
//		std::vector<NodeKeyData>	nodeKeys;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//	struct Animation
//	{
//		std::string					name;
//		float						secondsLength;
//		std::vector<Keyframe>		keyframes;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	std::vector<Node> nodes;
//	std::vector<Mesh> meshes;
//	std::vector<Material> materials;
//	std::vector<Animation> animations;
//
//	char dirName[256];
//};