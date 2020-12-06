#pragma once
#pragma once
#pragma once

#include <DirectXMath.h>
#include <memory>
#include "GeometricPrimitive.h"
#include <list>
#include <algorithm>
#include <vector>
#include "easing.h"
#include "BlendState.h"
#include "Board.h"
#include <string>
class ParticleObj //geometric_primitive ver
{
public:

	std::shared_ptr<GeometricPrimitive> obj;

	DirectX::XMFLOAT3	pos;
	DirectX::XMFLOAT3	speed;
	DirectX::XMFLOAT3	accel;
	DirectX::XMFLOAT3	angle;
	float	scale;
	DirectX::XMFLOAT4	color;
	float w;

	bool exist;
	int  dispTimer;
	int  dispMaxTimer;
	virtual void Init(const DirectX::XMFLOAT3& pos, std::shared_ptr<GeometricPrimitive> obj) = 0;
	virtual void Update(float elapsed_time) = 0;
	void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir);

};
class ParticleBoard//BillBoard ver
{
public:
	std::shared_ptr<Board> board;

	DirectX::XMFLOAT3	pos = { 0,0,0 };
	DirectX::XMFLOAT3	pos2 = { 0,0,0 };
	DirectX::XMFLOAT3	speed = { 0,0,0 };
	DirectX::XMFLOAT3	accel = { 0,0,0 };
	DirectX::XMFLOAT3	angle = { 0,0,0 };
	float	scale = 0;
	DirectX::XMFLOAT4	color = { 0,0,0,0 };
	float w = 0;

	bool exist = false;
	int  dispTimer = 0;
	int  dispMaxTimer = 0;;
	virtual void Init(const DirectX::XMFLOAT3& pos, std::shared_ptr<Board> board) = 0;
	virtual void Update(float elapsed_time) = 0;
	void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir);
};
class ParticleManager
{
public:
	//setter
	void SetTargetPos(const DirectX::XMFLOAT3 pos) { targetPos = pos; }
	//getter
	const DirectX::XMFLOAT3& GetTargetPos() { return targetPos; }
private:

	std::list<std::shared_ptr<ParticleObj>>particleObjList;
	std::list<std::shared_ptr<ParticleBoard>>particleBoardList;

	std::vector<std::shared_ptr<GeometricPrimitive>> obj;
	std::vector<std::shared_ptr<Board>> board;
	static ParticleManager* instance;

	DirectX::XMFLOAT3 targetPos = { 0,0,0 };
	std::vector<std::string>name;
	struct ParticleData
	{
		DirectX::XMFLOAT3	speed = { 0,0,0 };
		DirectX::XMFLOAT3	accel = { 0,0,0 };
		DirectX::XMFLOAT3	angle = { 0,0,0 };
		float	scale = 1.5f;
		DirectX::XMFLOAT4	color = { 1,1,1,100 };
		DirectX::XMFLOAT3 v = { 0, 0, 0 };
		int  dispTimer = 0;;
		int  dispMaxTimer = 100;
		int amount = 0;;

		int texnum = 0;;

		DirectX::XMFLOAT3  plusPos = { 0,0,0 };
	};
public:
	enum  tex //sprite 追加
	{
		Siro,
		Koa,
		Slash,
		Lp,
		Circle1,
		Circle2,
		Fire1,
		Fire2,
		Smoke,
	};
	enum  geo//geometric_primitive　追加
	{
		cube,
		geoSphere

	};

	ParticleData Data;

	void Create(ID3D11Device* device)
	{
		if (instance != nullptr)return;
		instance = new ParticleManager;
		instance->Init(device);
	}


	void Init(ID3D11Device* device);

	void Update(float elapsed_time);

	void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir);


	bool Add_Obj(std::shared_ptr<ParticleObj>, geo i, const DirectX::XMFLOAT3& pos = DirectX::XMFLOAT3(0.f, 0.f, 0.f)); //引数(Particle種類,素材種類,場所)
	bool Add_Board(std::shared_ptr<ParticleBoard>, tex i, const DirectX::XMFLOAT3& pos = DirectX::XMFLOAT3(0.f, 0.f, 0.f)); //引数(Particle種類,素材種類,場所)

	void Load();


	static ParticleManager* getInstance()
	{

		return instance;
	}

	void Destroy()
	{
		delete instance;
		instance = nullptr;
	}
};
#define	pParticleManager	ParticleManager::getInstance()

//加速
class ParticleAccel : public ParticleBoard
{
public:

	void Init(const DirectX::XMFLOAT3& pos, std::shared_ptr<Board> board);
	void Update(float elapsedTime);
};
//攻撃ヒット時
class ParticleHit : public ParticleBoard
{
public:

	void Init(const DirectX::XMFLOAT3& pos, std::shared_ptr<Board> board);
	void Update(float elapsedTime);

};
//エネミー溜め攻撃時
class ParticleCharge : public ParticleBoard
{
public:

	void Init(const DirectX::XMFLOAT3& pos, std::shared_ptr<Board> board);
	void Update(float elapsedTime);

};
class ParticleTst : public ParticleBoard
{
public:

	void Init(const DirectX::XMFLOAT3& pos, std::shared_ptr<Board> board);
	void Update(float elapsedTime);
};
//エネミーテレポート移動
class ParticleTeleport : public ParticleBoard
{
public:

	void Init(const DirectX::XMFLOAT3& pos, std::shared_ptr<Board> board);
	void Update();
};
