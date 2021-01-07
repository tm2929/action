#pragma once
#include "Character.h"
#include "HitObjData.h"


class Stage
{
public:
	Stage();
	void Update(float elapsedTime);
	void Imgui();

	//ÉQÉbÉ^Å[

	std::shared_ptr<Character> GetHitLeft() { return hitLeft; }
	std::shared_ptr<Character> GetLeftObj1() { return tstLeft1; }
	std::shared_ptr<Character> GetLeftObj2() { return tstLeft2; }
	std::shared_ptr<Character> GetRightObj1() { return tstRight1; }
	std::shared_ptr<Character> GetRightObj2() { return tstRight2; }
	std::shared_ptr<Character> GetFrontObj1() { return tstFront1; }
	std::shared_ptr<Character> GetFrontObj2() { return tstFront2; }
	std::shared_ptr<Character> GetBackObj1() { return tstBack1; }
	std::shared_ptr<Character> GetBackObj2() { return tstBack2; }
	std::shared_ptr<Character> GetHaka1() { return haka1; }
	std::shared_ptr<Character> GetHaka2() { return haka2; }
	std::shared_ptr<Obj3D> GetFloor() { return floor; }

	const AABB& GetLeft1Cube()const { return left1Cube; }
	const AABB& GetLeft2Cube()const { return left2Cube; }
	const AABB& GetRight1Cube()const { return right1Cube; }
	const AABB& GetRight2Cube()const { return right2Cube; }
	const AABB& GetFront1Cube()const { return front1Cube; }
	const AABB& GetFront2Cube()const { return front2Cube; }
	const AABB& GetHaka1Cube()const { return haka1Cube; }
	const AABB& GetHaka2Cube()const { return haka2Cube; }
	const AABB& GetBack1Cube()const { return back1Cube; }
	const AABB& GetBack2Cube()const { return back2Cube; }

private:
	DirectX::XMFLOAT3 max = { 0,0,0 };
	DirectX::XMFLOAT3 min = { 0,0,0 };
	DirectX::XMFLOAT3 pos = { 0,0,0 };

	std::shared_ptr<Character> tstLeft1;
	std::shared_ptr<Character> tstLeft2;
	std::shared_ptr<Character> hitLeft;
	std::shared_ptr<Character> tstRight1;
	std::shared_ptr<Character> tstRight2;
	std::shared_ptr<Character> tstFront1;
	std::shared_ptr<Character> tstFront2;
	std::shared_ptr<Character> tstBack1;
	std::shared_ptr<Character> tstBack2;
	std::shared_ptr<Character> haka1;
	std::shared_ptr<Character> haka2;
	std::shared_ptr<Character> iriguti;

	std::shared_ptr<Obj3D>floor;

	//ìñÇΩÇËîªíË
	AABB left1Cube;
	AABB left2Cube;
	AABB right1Cube;
	AABB right2Cube;
	AABB front1Cube;
	AABB front2Cube;
	AABB haka1Cube;
	AABB back1Cube;
	AABB back2Cube;
	AABB haka2Cube;
};


//#define ASSETSCOUNT  20
//class StageData
//{
//public:
//	std::shared_ptr<Obj3D> obj;
//	DirectX::XMFLOAT3 pos;
//};
//
//class StageManager
//{
//public:
//	void Create(ID3D11Device* device)
//	{
//		if (stageManager != nullptr)return;
//		stageManager = new StageManager(device);
//	}
//	void SetObj(int num, DirectX::XMFLOAT3 pos);
//	void Imgui();
//	void Destroy()
//	{
//		delete stageManager;
//		stageManager = nullptr;
//		stageData.clear();
//	}
//	static StageManager& getInstance()
//	{
//		return *stageManager;
//	}
//	std::vector<StageData> GetStageData() { return stageData; }
//private:
//	int fileNum = 1;
//	DirectX::XMFLOAT3 pos;
//	std::vector<StageData>stageData;
//	int plusX;
//	int plusY;
//	int plusZ;
//	static StageManager* stageManager;
//	StageManager(ID3D11Device* device);
//};