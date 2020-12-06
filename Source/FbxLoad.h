//// UNIT.02
//#pragma once
//
//#include <fbxsdk.h> 
//#include "ModelData.h"
//
//class FbxLoader
//{
//public:
//	// FBX�t�@�C�����烂�f���f�[�^�ǂݍ���
//	bool Load(const char* filename, ModelData& data);
//
//
//private:
//	// �m�[�h�f�[�^���\�z
//	void LoadNodes(FbxNode* fbxNode, ModelData& data, int parentNodeIndex);
//	void LoadNode(FbxNode* fbxNode, ModelData& data, int parentNodeIndex);
//
//	// ���b�V���f�[�^��ǂݍ���
//	void LoadMeshes(FbxNode* fbxNode, ModelData& data );
//	void LoadMesh(FbxNode* fbxNode, FbxMesh* fbxMesh, ModelData& data);
//
//	// �}�e���A���f�[�^��ǂݍ���
//	void LoadMaterials(const char* dirname, FbxScene* fbxScene, ModelData& data);
//	void LoadMaterial(const char* dirname, FbxSurfaceMaterial* fbxSurfaceMaterial, ModelData& data);
//
//	// �A�j���[�V�����f�[�^��ǂݍ���
//	void LoadAnimations(FbxScene* fbxScene, ModelData& data);
//
//	// �m�[�h�p�X�擾
//	std::string GetNodePath(FbxNode* fbxNode) const;
//
//	// �C���f�b�N�X�̌���
//	int FindNodeIndex(ModelData& data, const char* name);
//	int FindMaterialIndex(FbxScene* fbxScene, const FbxSurfaceMaterial* fbxSurfaceMaterial);
//
//};
