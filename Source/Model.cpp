#include "Logger.h"
#include "Misc.h"
#include "Model.h"

Model::Model(std::shared_ptr<ModelResource>& resource)
{
	this->resource = resource;

	// ノード
	const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

	nodes.resize(resNodes.size());
	for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
	{
		auto&& src = resNodes.at(nodeIndex);
		auto&& dst = nodes.at(nodeIndex);

		dst.name = src.name.c_str();
		dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;

		if (dst.parent != nullptr)
		{
			dst.parent->children.emplace_back(&dst);
		}
	}

	blendTime = 5;
}

// アニメーション再生
void Model::PlayAnimation(int animationIndex, bool loop)
{
	//変更前アニメーションを保存
	prevAnimation = currentAnimation;
	//アニメーションをセット
	currentAnimation = animationIndex;
	loopAnimation = loop;
	endAnimation = false;
	currentSeconds = 0.0f;
}

// アニメーション計算
void Model::UpdateAnimation(float elapsedTime)
{
	if (currentAnimation < 0)
	{
		return;
	}

	if (resource->GetAnimations().empty())
	{
		return;
	}

	//アニメーションが切り替わった場合
	if (currentAnimation != prevAnimation)
	{
		//モーションブレンドを開始
		blendFlag = true;
		prevAnimation = currentAnimation;//変わった瞬間のみはいるように
		blendSeconds = 0;
		endAnimation = false;
	}
	//モーションブレンド
	if (blendFlag)
	{
		const ModelResource::Animation& animation = resource->GetAnimations().at(currentAnimation);
		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		const ModelResource::Keyframe& keyframe = keyframes.at(1);

		int nodeCount = static_cast<int>(nodes.size());

		float rate = (1.0f / blendTime);
		for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
		{
			// ２つのキーフレーム間の補完計算
			const ModelResource::NodeKeyData& key = keyframe.nodeKeys.at(nodeIndex);
			Model::Node& node = nodes[nodeIndex];

			DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&node.scale);
			DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key.scale);
			DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&node.rotate);
			DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key.rotate);
			DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&node.translate);
			DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key.translate);

			//補完
			DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
			DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
			DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

			DirectX::XMStoreFloat3(&node.scale, S);
			DirectX::XMStoreFloat4(&node.rotate, R);
			DirectX::XMStoreFloat3(&node.translate, T);
		}
		blendSeconds++;

		//ブレンド終了処理
		if (blendSeconds > blendTime)
		{
			blendSeconds = 0;
			blendFlag = false;

			currentSeconds = keyframe.seconds;
		}
		else
		{
			return;
		}
	}
	const ModelResource::Animation& animation = resource->GetAnimations().at(currentAnimation);

	const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		// 現在の時間がどのキーフレームの間にいるか判定する
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		if (currentSeconds >= keyframe0.seconds && currentSeconds < keyframe1.seconds)
		{
			float rate = (currentSeconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

			assert(nodes.size() == keyframe0.nodeKeys.size());
			assert(nodes.size() == keyframe1.nodeKeys.size());
			int nodeCount = static_cast<int>(nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				// ２つのキーフレーム間の補完計算
				const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

				Node& node = nodes[nodeIndex];

				DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
				DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
				DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
				DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
				DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
				DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

				DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
				DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
				DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

				DirectX::XMStoreFloat3(&node.scale, S);
				DirectX::XMStoreFloat4(&node.rotate, R);
				DirectX::XMStoreFloat3(&node.translate, T);
			}
			break;
		}
	}

	// 最終フレーム処理
	if (endAnimation)
	{
		endAnimation = false;
		currentAnimation = -1;
		return;
	}

	// 時間経過
	currentSeconds += elapsedTime;
	if (currentSeconds >= animation.secondsLength)
	{
		if (loopAnimation)
		{
			currentSeconds -= animation.secondsLength;
		}
		else
		{
			currentSeconds = animation.secondsLength;
			endAnimation = true;
		}
	}
}

// ローカル変換行列計算
void Model::CalculateLocalTransform()
{
	for (Node& node : nodes)
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);

		DirectX::XMStoreFloat4x4(&node.localTransform, S * R * T);
	}
}

// ワールド変換行列計算
void Model::CalculateWorldTransform(const DirectX::XMMATRIX& WorldTransform)
{
	for (Node& node : nodes)
	{
		DirectX::XMMATRIX LocalTransform = DirectX::XMLoadFloat4x4(&node.localTransform);
		if (node.parent != nullptr)
		{
			DirectX::XMMATRIX ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
			DirectX::XMStoreFloat4x4(&node.worldTransform, LocalTransform * ParentTransform);
		}
		else
		{
			DirectX::XMStoreFloat4x4(&node.worldTransform, LocalTransform * WorldTransform);
		}
	}
}
int Model::RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition,
	DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float* outLength)
{
	int ret = -1;
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);
	float neart;
	DirectX::XMStoreFloat(&neart, length);

	DirectX::XMVECTOR position, normal;
	for (const auto& mesh : GetResource()->GetMeshes())
	{
		for (const auto& face : mesh.faces)
		{
			//面頂点取得
			DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&face.position[0]);
			DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&face.position[1]);
			DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&face.position[2]);
			//3辺算出
			DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
			DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
			DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);
			//外積による法線算出
			DirectX::XMVECTOR n = DirectX::XMVector3Cross(ab, bc);
			//内積の結果がプラスならば裏向き
			DirectX::XMVECTOR dot = DirectX::XMVector3Dot(dir, n);
			float fdot;
			DirectX::XMStoreFloat(&fdot, dot);
			if (fdot >= 0)continue;
			//交点算出
			DirectX::XMVECTOR cp;
			DirectX::XMVECTOR as = DirectX::XMVectorSubtract(a, start);
			DirectX::XMVECTOR d1 = DirectX::XMVector3Dot(as, n);
			DirectX::XMVECTOR x = DirectX::XMVectorDivide(d1, dot);//割り算
																   //距離がnearより遠いか判定
			float fleng;
			DirectX::XMVECTOR face_leng = DirectX::XMVector3Length(x);

			DirectX::XMStoreFloat(&fleng, x);
			if (neart * neart < fleng * fleng)continue;

			cp = DirectX::XMVectorAdd(start, DirectX::XMVectorMultiply(dir, x));//足し算、かけ算
																				//内点判定
			DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(a, cp);
			DirectX::XMVECTOR temp = DirectX::XMVector3Cross(v1, ab);
			DirectX::XMVECTOR work = DirectX::XMVector3Dot(temp, n);
			float fwork;
			DirectX::XMStoreFloat(&fwork, work);
			if (fwork < 0.0f)continue;
			DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(b, cp);
			temp = DirectX::XMVector3Cross(v2, bc);
			work = DirectX::XMVector3Dot(temp, n);
			DirectX::XMStoreFloat(&fwork, work);
			if (fwork < 0.0f)continue;
			DirectX::XMVECTOR v3 = DirectX::XMVectorSubtract(c, cp);
			temp = DirectX::XMVector3Cross(v3, ca);
			work = DirectX::XMVector3Dot(temp, n);
			DirectX::XMStoreFloat(&fwork, work);
			if (fwork < 0.0f)continue;

			//情報保存
			position = cp;
			normal = n;
			ret = face.materialIndex;
			neart = fleng;
		}
	}
	if (ret != -1)
	{
		DirectX::XMStoreFloat3(outPosition, position);
		DirectX::XMStoreFloat3(outNormal, normal);
	}
	*outLength = neart;
	return ret;
}