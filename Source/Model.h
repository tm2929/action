#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "ModelResource.h"

class Model
{
public:
	Model(std::shared_ptr<ModelResource>& resource);
	~Model() {}

	struct Node
	{
		const char* name;
		Node* parent;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
		DirectX::XMFLOAT4X4	localTransform;
		DirectX::XMFLOAT4X4	worldTransform;

		std::vector<Node*>	children;
	};
	struct Face
	{
		DirectX::XMFLOAT3 position[3];
		int materialIndex;
	};

	// アニメーション
	bool IsPlayAnimation() const { return currentAnimation >= 0; }
	void PlayAnimation(int animationIndex, bool loop = false);
	void UpdateAnimation(float elapsedTime);

	int RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float* outLength);
	// 行列計算
	void CalculateLocalTransform();
	void CalculateWorldTransform(const DirectX::XMMATRIX& WorldTransform);

	//セッター
	void SetBlendTime(const float t) { blendTime = t; }

	//ゲッター
	const std::vector<Node>& GetNodes() const { return nodes; }
	std::vector<Node>& GetNodes() { return nodes; }
	const ModelResource* GetResource() const { return resource.get(); }
	const bool GetBlendFlag() const { return blendFlag; }
	const float GetBlendTime() const { return blendTime; }
	// アニメーション時間
	float GetAnimationSeconds() const { return currentSeconds; }
	void SetAnimationSeconds(float seconds) { currentSeconds = seconds; }

private:
	std::shared_ptr<ModelResource>	resource;
	std::vector<Node>				nodes;
	std::vector<Face>				faces;
	int								currentAnimation = -1;
	int								prevAnimation = 0;
	bool							blendFlag = false;
	float							currentSeconds = 0.0f;
	float							blendSeconds = 0.0f;
	float							blendTime = 1.0f;
	bool							loopAnimation = false;
	bool							endAnimation = false;
};
