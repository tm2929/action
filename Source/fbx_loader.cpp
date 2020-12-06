
#include <functional>
#include "fbx_loader.h"
#include "convert_Fbx_to_DirectX.h"

// FBXファイルからモデルデータ読み込み
bool FbxLoader::Load(const char* fileName, ModelData& data)
{
	FbxManager* fbxManager = FbxManager::Create();

	// Fbxに対する入出力を定義する
	FbxIOSettings* fbxIos = FbxIOSettings::Create(fbxManager, IOSROOT); // 特別な理由がない限りIOSROOTを指定
	fbxManager->SetIOSettings(fbxIos);

	// インポータを生成
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	bool result = fbxImporter->Initialize(fileName, -1, fbxManager->GetIOSettings()); // -1でファイルフォーマット自動判定
	if (result)
	{
		// SceneオブジェクトにFBXファイル内の情報を流し込む
		FbxScene* fbxScene = FbxScene::Create(fbxManager, "scene");
		fbxImporter->Import(fbxScene);
		fbxImporter->Destroy();	 // シーンを流し込んだらImporterは解放してOK

		// ジオメトリを三角形化しておく
		FbxGeometryConverter fbxGeometryConverter(fbxManager);
		fbxGeometryConverter.Triangulate(fbxScene, true);
		fbxGeometryConverter.RemoveBadPolygonsFromMeshes(fbxScene);

		// ディレクトリパス取得
		::_splitpath_s(fileName, nullptr, 0, data.dirName, 256, nullptr, 0, nullptr, 0);

		// モデル構築
		result = BuildModel(data.dirName, fbxScene, data);
	}
	else
	{
		assert(!"Not Found FbxFile !");
	}

	// マネージャー解放
	fbxManager->Destroy(); // 関連するすべてのオブジェクトが解放される

	return result;
}

// FBXシーンからモデルデータを構築する
bool FbxLoader::BuildModel(const char* dirName, FbxScene* fbxScene, ModelData& data)
{
	FbxNode* fbxRootNode = fbxScene->GetRootNode();

	BuildNodes(fbxRootNode, data, -1);
	BuildMeshes(fbxRootNode, data);
	BuildMaterials(dirName, fbxScene, data);
	BuildAnimations(fbxScene, data);

	return true;
}

// FBXノードを再帰的に辿ってデータを構築する
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

	// 再帰的に子ノードを処理する
	parentNodeIndex = static_cast<int>(data.nodes.size() - 1);
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		BuildNodes(fbxNode->GetChild(i), data, parentNodeIndex);
	}
}

// FBXノードからノードデータを構築する
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

// FBXノードを再帰的に辿ってメッシュデータを構築する
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

	// 再帰的に子ノードを処理する
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		BuildMeshes(fbxNode->GetChild(i), data);
	}
}

// FBXメッシュからメッシュデータを構築する
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

    // 複数メッシュ
    FbxAMatrix globalTransform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
    mesh.globalTransform = FbxAMatrixToFloat4x4(globalTransform);

	// サブセットのマテリアル設定
	for (int fbxMaterialIndex = 0; fbxMaterialIndex < fbxMaterialCount; ++fbxMaterialIndex)
	{
		const FbxSurfaceMaterial* fbxSurfaceMaterial = fbxNode->GetMaterial(fbxMaterialIndex);

		ModelData::Subset& subset = mesh.subsets.at(fbxMaterialIndex);
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
		for (ModelData::Subset& subset : mesh.subsets)
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
		boneInfluences.resize(fbxControlPointCount);

		FbxAMatrix fbxGeometricTransform(
			fbxNode->GetGeometricTranslation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricRotation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricScaling(FbxNode::eSourcePivot)
		);

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

	// UVセット名
	FbxStringList fbxUVNames;
	fbxMesh->GetUVSetNames(fbxUVNames);

	// 頂点データ
	const FbxVector4* fbxControlPoints = fbxMesh->GetControlPoints();
	ModelData::Face face;
    DirectX::XMMATRIX meshTransform = DirectX::XMLoadFloat4x4(&mesh.globalTransform);

	for (int fbxPolygonIndex = 0; fbxPolygonIndex < fbxPolygonCount; ++fbxPolygonIndex)
	{
		// ポリゴンに適用されているマテリアルインデックスを取得する
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
			
                // 面ごとの頂点データ    
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

// FBXシーン内のFBXマテリアルからマテリアルデータを構築する
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

// FBXマテリアルからマテリアルデータを構築する
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

// アニメーションデータを構築
void FbxLoader::BuildAnimations(FbxScene* fbxScene, ModelData& data)
{
	// すべてのアニメーション名を取得
	FbxArray<FbxString*> fbxAnimStackNames;
	fbxScene->FillAnimStackNameArray(fbxAnimStackNames);

	int fbxAnimationCount = fbxAnimStackNames.Size();
	for (int fbxAnimationIndex = 0; fbxAnimationIndex < fbxAnimationCount; ++fbxAnimationIndex)
	{
		data.animations.emplace_back(ModelData::Animation());
		ModelData::Animation& animation = data.animations.back();

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

		// アニメーションデータを抽出する
		animation.secondsLength = frameCount * samplingTime;
		animation.keyFrames.resize(frameCount + 1);

		float seconds = 0.0f;
		ModelData::KeyFrame* keyFrame = animation.keyFrames.data();
		size_t fbxNodeCount = fbxNodes.size();
		//FbxTime fbx_current_time = fbx_start_time;
		for (FbxTime fbxCurrentTime = fbxStartTime; fbxCurrentTime < fbxEndTime; fbxCurrentTime += fbxSamplingStep, ++keyFrame)
		{
			// キーフレーム毎の姿勢データを取り出す。
			keyFrame->seconds = seconds;
			keyFrame->nodeKeys.resize(fbxNodeCount);
			for (size_t fbxNodeIndex = 0; fbxNodeIndex < fbxNodeCount; ++fbxNodeIndex)
			{
				ModelData::NodeKeyData& keyData = keyFrame->nodeKeys.at(fbxNodeIndex);
				FbxNode* fbxNode = fbxNodes.at(fbxNodeIndex);

				// 指定時間のローカル行列からスケール値、回転値、移動値を取り出す。
				const FbxAMatrix& fbxLocalTransform = fbxNode->EvaluateLocalTransform(fbxCurrentTime);

				keyData.scale = FbxDouble4ToFloat3(fbxLocalTransform.GetS());
				keyData.rotate = FbxDouble4ToFloat4(fbxLocalTransform.GetQ());
				keyData.translate = FbxDouble4ToFloat3(fbxLocalTransform.GetT());
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

// ノードインデックスを取得する
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

// マテリアルインデックスを取得する
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

