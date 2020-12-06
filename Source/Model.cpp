#include "Logger.h"
#include "Misc.h"
#include "Model.h"

Model::Model(std::shared_ptr<ModelResource>& resource)
{
	this->resource = resource;

	// �m�[�h
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

// �A�j���[�V�����Đ�
void Model::PlayAnimation(int animationIndex, bool loop)
{
	//�ύX�O�A�j���[�V������ۑ�
	prevAnimation = currentAnimation;
	//�A�j���[�V�������Z�b�g
	currentAnimation = animationIndex;
	loopAnimation = loop;
	endAnimation = false;
	currentSeconds = 0.0f;
}

// �A�j���[�V�����v�Z
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

	//�A�j���[�V�������؂�ւ�����ꍇ
	if (currentAnimation != prevAnimation)
	{
		//���[�V�����u�����h���J�n
		blendFlag = true;
		prevAnimation = currentAnimation;//�ς�����u�Ԃ݂̂͂���悤��
		blendSeconds = 0;
		endAnimation = false;
	}
	//���[�V�����u�����h
	if (blendFlag)
	{
		const ModelResource::Animation& animation = resource->GetAnimations().at(currentAnimation);
		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		const ModelResource::Keyframe& keyframe = keyframes.at(1);

		int nodeCount = static_cast<int>(nodes.size());

		float rate = (1.0f / blendTime);
		for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
		{
			// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
			const ModelResource::NodeKeyData& key = keyframe.nodeKeys.at(nodeIndex);
			Model::Node& node = nodes[nodeIndex];

			DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&node.scale);
			DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key.scale);
			DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&node.rotate);
			DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key.rotate);
			DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&node.translate);
			DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key.translate);

			//�⊮
			DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
			DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
			DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

			DirectX::XMStoreFloat3(&node.scale, S);
			DirectX::XMStoreFloat4(&node.rotate, R);
			DirectX::XMStoreFloat3(&node.translate, T);
		}
		blendSeconds++;

		//�u�����h�I������
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
		// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
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
				// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
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

	// �ŏI�t���[������
	if (endAnimation)
	{
		endAnimation = false;
		currentAnimation = -1;
		return;
	}

	// ���Ԍo��
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

// ���[�J���ϊ��s��v�Z
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

// ���[���h�ϊ��s��v�Z
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
			//�ʒ��_�擾
			DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&face.position[0]);
			DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&face.position[1]);
			DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&face.position[2]);
			//3�ӎZ�o
			DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
			DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
			DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);
			//�O�ςɂ��@���Z�o
			DirectX::XMVECTOR n = DirectX::XMVector3Cross(ab, bc);
			//���ς̌��ʂ��v���X�Ȃ�Η�����
			DirectX::XMVECTOR dot = DirectX::XMVector3Dot(dir, n);
			float fdot;
			DirectX::XMStoreFloat(&fdot, dot);
			if (fdot >= 0)continue;
			//��_�Z�o
			DirectX::XMVECTOR cp;
			DirectX::XMVECTOR as = DirectX::XMVectorSubtract(a, start);
			DirectX::XMVECTOR d1 = DirectX::XMVector3Dot(as, n);
			DirectX::XMVECTOR x = DirectX::XMVectorDivide(d1, dot);//����Z
																   //������near��艓��������
			float fleng;
			DirectX::XMVECTOR face_leng = DirectX::XMVector3Length(x);

			DirectX::XMStoreFloat(&fleng, x);
			if (neart * neart < fleng * fleng)continue;

			cp = DirectX::XMVectorAdd(start, DirectX::XMVectorMultiply(dir, x));//�����Z�A�����Z
																				//���_����
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

			//���ۑ�
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