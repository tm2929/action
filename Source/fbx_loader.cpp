
#include <functional>
#include "fbx_loader.h"
#include "convert_Fbx_to_DirectX.h"

// FBX�t�@�C�����烂�f���f�[�^�ǂݍ���
bool FbxLoader::Load(const char* fileName, ModelData& data)
{
	FbxManager* fbxManager = FbxManager::Create();

	// Fbx�ɑ΂�����o�͂��`����
	FbxIOSettings* fbxIos = FbxIOSettings::Create(fbxManager, IOSROOT); // ���ʂȗ��R���Ȃ�����IOSROOT���w��
	fbxManager->SetIOSettings(fbxIos);

	// �C���|�[�^�𐶐�
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	bool result = fbxImporter->Initialize(fileName, -1, fbxManager->GetIOSettings()); // -1�Ńt�@�C���t�H�[�}�b�g��������
	if (result)
	{
		// Scene�I�u�W�F�N�g��FBX�t�@�C�����̏��𗬂�����
		FbxScene* fbxScene = FbxScene::Create(fbxManager, "scene");
		fbxImporter->Import(fbxScene);
		fbxImporter->Destroy();	 // �V�[���𗬂����񂾂�Importer�͉������OK

		// �W�I���g�����O�p�`�����Ă���
		FbxGeometryConverter fbxGeometryConverter(fbxManager);
		fbxGeometryConverter.Triangulate(fbxScene, true);
		fbxGeometryConverter.RemoveBadPolygonsFromMeshes(fbxScene);

		// �f�B���N�g���p�X�擾
		::_splitpath_s(fileName, nullptr, 0, data.dirName, 256, nullptr, 0, nullptr, 0);

		// ���f���\�z
		result = BuildModel(data.dirName, fbxScene, data);
	}
	else
	{
		assert(!"Not Found FbxFile !");
	}

	// �}�l�[�W���[���
	fbxManager->Destroy(); // �֘A���邷�ׂẴI�u�W�F�N�g����������

	return result;
}

// FBX�V�[�����烂�f���f�[�^���\�z����
bool FbxLoader::BuildModel(const char* dirName, FbxScene* fbxScene, ModelData& data)
{
	FbxNode* fbxRootNode = fbxScene->GetRootNode();

	BuildNodes(fbxRootNode, data, -1);
	BuildMeshes(fbxRootNode, data);
	BuildMaterials(dirName, fbxScene, data);
	BuildAnimations(fbxScene, data);

	return true;
}

// FBX�m�[�h���ċA�I�ɒH���ăf�[�^���\�z����
void FbxLoader::BuildNodes(FbxNode* fbxNode, ModelData& data, int parentNodeIndex)
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
		BuildNode(fbxNode, data, parentNodeIndex);
		break;
	}

	// �ċA�I�Ɏq�m�[�h����������
	parentNodeIndex = static_cast<int>(data.nodes.size() - 1);
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		BuildNodes(fbxNode->GetChild(i), data, parentNodeIndex);
	}
}

// FBX�m�[�h����m�[�h�f�[�^���\�z����
void FbxLoader::BuildNode(FbxNode* fbxNode, ModelData& data, int parentNodeIndex)
{
	FbxAMatrix& fbxLocalTransform = fbxNode->EvaluateLocalTransform();

	ModelData::Node node;
	node.name = fbxNode->GetName();
	node.parentIndex = parentNodeIndex;
	node.scale = FbxDouble4ToFloat3(fbxLocalTransform.GetS());
	node.rotate = FbxDouble4ToFloat4(fbxLocalTransform.GetQ());
	node.translate = FbxDouble4ToFloat3(fbxLocalTransform.GetT());

	data.nodes.emplace_back(node);
}

// FBX�m�[�h���ċA�I�ɒH���ă��b�V���f�[�^���\�z����
void FbxLoader::BuildMeshes(FbxNode* fbxNode, ModelData& data)
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
		BuildMesh(fbxNode, static_cast<FbxMesh*>(fbxNodeAttribute), data);
		break;
	}

	// �ċA�I�Ɏq�m�[�h����������
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		BuildMeshes(fbxNode->GetChild(i), data);
	}
}

// FBX���b�V�����烁�b�V���f�[�^���\�z����
void FbxLoader::BuildMesh(FbxNode* fbxNode, FbxMesh* fbxMesh, ModelData& data)
{
	int fbxControlPointCount = fbxMesh->GetControlPointsCount();
	int fbxPolygonVertexCount = fbxMesh->GetPolygonVertexCount();
	const int* fbxPolygonVertices = fbxMesh->GetPolygonVertices();

	int fbxMaterialCount = fbxNode->GetMaterialCount();
	int fbxPolygonCount = fbxMesh->GetPolygonCount();

	data.meshes.emplace_back(ModelData::Mesh());
	ModelData::Mesh& mesh = data.meshes.back();
	mesh.indices.resize(fbxPolygonCount * 3);
	mesh.subsets.resize(fbxMaterialCount > 0 ? fbxMaterialCount : 1);
	mesh.nodeIndex = FindNodeIndex(data, fbxNode->GetName());

    // �������b�V��
    FbxAMatrix globalTransform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
    mesh.globalTransform = FbxAMatrixToFloat4x4(globalTransform);

	// �T�u�Z�b�g�̃}�e���A���ݒ�
	for (int fbxMaterialIndex = 0; fbxMaterialIndex < fbxMaterialCount; ++fbxMaterialIndex)
	{
		const FbxSurfaceMaterial* fbxSurfaceMaterial = fbxNode->GetMaterial(fbxMaterialIndex);

		ModelData::Subset& subset = mesh.subsets.at(fbxMaterialIndex);
		subset.materialIndex = FindMaterialIndex(fbxNode->GetScene(), fbxSurfaceMaterial);
	}

	// �T�u�Z�b�g�̒��_�C���f�b�N�X�͈͐ݒ�
	if (fbxMaterialCount > 0)
	{
		for (int fbxPolygonIndex = 0; fbxPolygonIndex < fbxPolygonCount; ++fbxPolygonIndex)
		{
			int fbxMaterialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(fbxPolygonIndex);
			mesh.subsets.at(fbxMaterialIndex).indexCount += 3;
		}

		int offset = 0;
		for (ModelData::Subset& subset : mesh.subsets)
		{
			subset.startIndex = offset;
			offset += subset.indexCount;

			subset.indexCount = 0;
		}
	}

	// ���_�e���̓f�[�^
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
	// ���_�e���̓f�[�^�𒊏o����
	std::vector<BoneInfluence> boneInfluences;
	{
		boneInfluences.resize(fbxControlPointCount);

		FbxAMatrix fbxGeometricTransform(
			fbxNode->GetGeometricTranslation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricRotation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricScaling(FbxNode::eSourcePivot)
		);

		// �X�L�j���O�ɕK�v�ȏ����擾����
		int fbxDeformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int fbxDeformerIndex = 0; fbxDeformerIndex < fbxDeformerCount; ++fbxDeformerIndex)
		{
			FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(fbxDeformerIndex, FbxDeformer::eSkin));

			int fbxClusterCount = fbxSkin->GetClusterCount();
			for (int fbxClusterIndex = 0; fbxClusterIndex < fbxClusterCount; ++fbxClusterIndex)
			{
				FbxCluster* fbxCluster = fbxSkin->GetCluster(fbxClusterIndex);

				// ���_�e���̓f�[�^�𒊏o����
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

				// �{�[���ϊ��s��p�̋t�s��̌v�Z������
				{
					// ���b�V����Ԃ��烏�[���h��Ԃւ̕ϊ��s��
					FbxAMatrix fbxMeshSpaceTransform;
					fbxCluster->GetTransformMatrix(fbxMeshSpaceTransform);

					// �{�[����Ԃ��烏�[���h��Ԃւ̕ϊ��s��
					FbxAMatrix fbxBoneSpaceTransform;
					fbxCluster->GetTransformLinkMatrix(fbxBoneSpaceTransform);

					// �{�[���t�s����v�Z����
					const char* fbx_name = fbxNode->GetName();
					FbxAMatrix fbxInverseTransform = fbxBoneSpaceTransform.Inverse() * fbxMeshSpaceTransform * fbxGeometricTransform;



					FLOAT4X4 inverseTransform = FbxAMatrixToFloat4x4(fbxInverseTransform);
					mesh.inverseTransforms.emplace_back(inverseTransform);

					int nodeIndex = FindNodeIndex(data, fbxCluster->GetLink()->GetName());
					mesh.nodeIndices.emplace_back(nodeIndex);
				}
			}
		}
	}

	// UV�Z�b�g��
	FbxStringList fbxUVNames;
	fbxMesh->GetUVSetNames(fbxUVNames);

	// ���_�f�[�^
	const FbxVector4* fbxControlPoints = fbxMesh->GetControlPoints();
	ModelData::Face face;
    DirectX::XMMATRIX meshTransform = DirectX::XMLoadFloat4x4(&mesh.globalTransform);

	for (int fbxPolygonIndex = 0; fbxPolygonIndex < fbxPolygonCount; ++fbxPolygonIndex)
	{
		// �|���S���ɓK�p����Ă���}�e���A���C���f�b�N�X���擾����
		int fbxMaterialIndex = 0;
		if (fbxMaterialCount > 0)
		{
			fbxMaterialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(fbxPolygonIndex);
		}

		ModelData::Subset& subset = mesh.subsets.at(fbxMaterialIndex);
		const int indexOffset = subset.startIndex + subset.indexCount;
		face.materialIndex = fbxMaterialIndex;

		for (int fbxVertexIndex = 0; fbxVertexIndex < 3; ++fbxVertexIndex)
		{
			ModelData::Vertex vertex;

			int fbxControlPointIndex = fbxMesh->GetPolygonVertex(fbxPolygonIndex, fbxVertexIndex);
			// Position
			{
				vertex.position = FbxDouble4ToFloat3(fbxControlPoints[fbxControlPointIndex]);
			
                // �ʂ��Ƃ̒��_�f�[�^    
                DirectX::XMVECTOR localPosition = DirectX::XMLoadFloat3(&vertex.position);
                DirectX::XMVECTOR globalPosition = DirectX::XMVector3TransformCoord(
                    localPosition, meshTransform);

                DirectX::XMStoreFloat3(&face.position[fbxVertexIndex], globalPosition);
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
				vertex.boneWeight.w = boneInfluence.weights[3];
			}

			// Normal
			if (fbxMesh->GetElementNormalCount() > 0)
			{
				FbxVector4 fbxNormal;
				fbxMesh->GetPolygonVertexNormal(fbxPolygonIndex, fbxVertexIndex, fbxNormal);
				vertex.normal = FbxDouble4ToFloat3(fbxNormal);
			}
			else
			{
				vertex.normal = VECTOR3(1, 0, 0);
			}

			// Texcoord
			if (fbxMesh->GetElementUVCount() > 0)
			{
				bool fbxUnmappedUV;
				FbxVector2 fbxUV;
				fbxMesh->GetPolygonVertexUV(fbxPolygonIndex, fbxVertexIndex, fbxUVNames[0], fbxUV, fbxUnmappedUV);
				fbxUV[1] = 1.0 - fbxUV[1];
				vertex.texcoord = FbxDouble2ToFloat2(fbxUV);
			}
			else
			{
				vertex.texcoord = VECTOR2(0, 0);
			}
       
			mesh.indices.at(indexOffset + fbxVertexIndex) = static_cast<int>(mesh.vertices.size());
			mesh.vertices.emplace_back(vertex);
		}
        
        mesh.faces.push_back(face);
		subset.indexCount += 3;
	}
}

// FBX�V�[������FBX�}�e���A������}�e���A���f�[�^���\�z����
void FbxLoader::BuildMaterials(const char* dirname, FbxScene* fbxScene, ModelData& data)
{
	int fbxMaterialCount = fbxScene->GetMaterialCount();

	if (fbxMaterialCount > 0)
	{
		for (int fbxMaterialIndex = 0; fbxMaterialIndex < fbxMaterialCount; ++fbxMaterialIndex)
		{
			FbxSurfaceMaterial* fbxSurfaceMaterial = fbxScene->GetMaterial(fbxMaterialIndex);
			BuildMaterial(dirname, fbxSurfaceMaterial, data);
		}
	}
	else
	{
		ModelData::Material material;
		material.color = VECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		data.materials.emplace_back(material);
	}
}

// FBX�}�e���A������}�e���A���f�[�^���\�z����
void FbxLoader::BuildMaterial(const char* dirName, FbxSurfaceMaterial* fbxSurfaceMaterial, ModelData& data)
{
	bool ret = false;

	ModelData::Material material;

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
	FbxProperty fbxTransparencyFactorProperty = fbxSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if ()
	{
		FbxDouble fbxFactor = fbxTransparencyFactorProperty.Get<FbxDouble>();
		material.color.w = static_cast<float>(fbxFactor);
	}
#endif
	if (fbxDiffuseProperty.IsValid())
	{
		int fbxTextureCount = fbxDiffuseProperty.GetSrcObjectCount<FbxFileTexture>();
		if (fbxTextureCount > 0)
		{
			FbxFileTexture* fbxTexture = fbxDiffuseProperty.GetSrcObject<FbxFileTexture>();

			char fileName[256];
			::_makepath_s(fileName, 256, nullptr, dirName, fbxTexture->GetRelativeFileName(), nullptr);
			material.textureFileName = fileName;
		}
	}

	data.materials.emplace_back(material);
}

// �A�j���[�V�����f�[�^���\�z
void FbxLoader::BuildAnimations(FbxScene* fbxScene, ModelData& data)
{
	// ���ׂẴA�j���[�V���������擾
	FbxArray<FbxString*> fbxAnimStackNames;
	fbxScene->FillAnimStackNameArray(fbxAnimStackNames);

	int fbxAnimationCount = fbxAnimStackNames.Size();
	for (int fbxAnimationIndex = 0; fbxAnimationIndex < fbxAnimationCount; ++fbxAnimationIndex)
	{
		data.animations.emplace_back(ModelData::Animation());
		ModelData::Animation& animation = data.animations.back();

		// �A�j���[�V�����f�[�^�̃T���v�����O�ݒ�
		FbxTime::EMode fbxTimeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		FbxTime fbxFrameTime;
		fbxFrameTime.SetTime(0, 0, 0, 1, 0, fbxTimeMode);

		float samplingRate = static_cast<float>(fbxFrameTime.GetFrameRate(fbxTimeMode));
		float samplingTime = 1.0f / samplingRate;

		FbxString* fbxAnimStackName = fbxAnimStackNames.GetAt(fbxAnimationIndex);
		FbxAnimStack* fbxAnimStack = fbxScene->FindMember<FbxAnimStack>(fbxAnimStackName->Buffer());

		// �Đ�����A�j���[�V�������w�肷��B
		fbxScene->SetCurrentAnimationStack(fbxAnimStack);

		// �A�j���[�V�����̍Đ��J�n���ԂƍĐ��I�����Ԃ��擾����
		FbxTakeInfo* fbxTakeInfo = fbxScene->GetTakeInfo(fbxAnimStackName->Buffer());
		FbxTime fbxStartTime = fbxTakeInfo->mLocalTimeSpan.GetStart();
		FbxTime fbxEndTime = fbxTakeInfo->mLocalTimeSpan.GetStop();

		// ���o����f�[�^��60�t���[����ŃT���v�����O����
		FbxTime fbxSamplingStep;
		fbxSamplingStep.SetTime(0, 0, 1, 0, 0, fbxTimeMode);
		fbxSamplingStep = static_cast<FbxLongLong>(fbxSamplingStep.Get() * samplingTime);

		int startFrame = static_cast<int>(fbxStartTime.Get() / fbxSamplingStep.Get());
		int endFrame = static_cast<int>(fbxEndTime.Get() / fbxSamplingStep.Get());
		int frameCount = static_cast<int>((fbxEndTime.Get() - fbxStartTime.Get()) / fbxSamplingStep.Get());

		// �A�j���[�V�����̑ΏۂƂȂ�m�[�h��񋓂���
		std::vector<FbxNode*> fbxNodes;
		std::function<void(FbxNode*)> traverse = [&](FbxNode* fbxNode)
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
				fbxNodes.emplace_back(fbxNode);
				break;
			}

			for (int i = 0; i < fbxNode->GetChildCount(); i++)
			{
				traverse(fbxNode->GetChild(i));
			}
		};
		traverse(fbxScene->GetRootNode());

		// �A�j���[�V�����f�[�^�𒊏o����
		animation.secondsLength = frameCount * samplingTime;
		animation.keyFrames.resize(frameCount + 1);

		float seconds = 0.0f;
		ModelData::KeyFrame* keyFrame = animation.keyFrames.data();
		size_t fbxNodeCount = fbxNodes.size();
		//FbxTime fbx_current_time = fbx_start_time;
		for (FbxTime fbxCurrentTime = fbxStartTime; fbxCurrentTime < fbxEndTime; fbxCurrentTime += fbxSamplingStep, ++keyFrame)
		{
			// �L�[�t���[�����̎p���f�[�^�����o���B
			keyFrame->seconds = seconds;
			keyFrame->nodeKeys.resize(fbxNodeCount);
			for (size_t fbxNodeIndex = 0; fbxNodeIndex < fbxNodeCount; ++fbxNodeIndex)
			{
				ModelData::NodeKeyData& keyData = keyFrame->nodeKeys.at(fbxNodeIndex);
				FbxNode* fbxNode = fbxNodes.at(fbxNodeIndex);

				// �w�莞�Ԃ̃��[�J���s�񂩂�X�P�[���l�A��]�l�A�ړ��l�����o���B
				const FbxAMatrix& fbxLocalTransform = fbxNode->EvaluateLocalTransform(fbxCurrentTime);

				keyData.scale = FbxDouble4ToFloat3(fbxLocalTransform.GetS());
				keyData.rotate = FbxDouble4ToFloat4(fbxLocalTransform.GetQ());
				keyData.translate = FbxDouble4ToFloat3(fbxLocalTransform.GetT());
			}
			seconds += samplingTime;
		}
	}

	// ��n��
	for (int i = 0; i < fbxAnimationCount; i++)
	{
		delete fbxAnimStackNames[i];
	}
}

// �m�[�h�C���f�b�N�X���擾����
int FbxLoader::FindNodeIndex(ModelData& data, const char* name)
{
	for (size_t i = 0; i < data.nodes.size(); ++i)
	{
		if (data.nodes[i].name == name)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

// �}�e���A���C���f�b�N�X���擾����
int FbxLoader::FindMaterialIndex(FbxScene* fbxScene, const FbxSurfaceMaterial* fbxSurfaceMaterial)
{
	int fbx_material_count = fbxScene->GetMaterialCount();

	for (int i = 0; i < fbx_material_count; ++i)
	{
		if (fbxScene->GetMaterial(i) == fbxSurfaceMaterial)
		{
			return i;
		}
	}
	return -1;
}

