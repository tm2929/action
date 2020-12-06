//// UNIT.02
//#pragma once
//
//#include <fbxsdk.h> 
//#include "ModelData.h"
//
//class FbxLoader
//{
//public:
//	// FBXファイルからモデルデータ読み込み
//	bool Load(const char* filename, ModelData& data);
//
//
//private:
//	// ノードデータを構築
//	void LoadNodes(FbxNode* fbxNode, ModelData& data, int parentNodeIndex);
//	void LoadNode(FbxNode* fbxNode, ModelData& data, int parentNodeIndex);
//
//	// メッシュデータを読み込み
//	void LoadMeshes(FbxNode* fbxNode, ModelData& data );
//	void LoadMesh(FbxNode* fbxNode, FbxMesh* fbxMesh, ModelData& data);
//
//	// マテリアルデータを読み込み
//	void LoadMaterials(const char* dirname, FbxScene* fbxScene, ModelData& data);
//	void LoadMaterial(const char* dirname, FbxSurfaceMaterial* fbxSurfaceMaterial, ModelData& data);
//
//	// アニメーションデータを読み込み
//	void LoadAnimations(FbxScene* fbxScene, ModelData& data);
//
//	// ノードパス取得
//	std::string GetNodePath(FbxNode* fbxNode) const;
//
//	// インデックスの検索
//	int FindNodeIndex(ModelData& data, const char* name);
//	int FindMaterialIndex(FbxScene* fbxScene, const FbxSurfaceMaterial* fbxSurfaceMaterial);
//
//};
