#pragma once
#include <DirectXMath.h>
#include <vector>

#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

class EffectObj
{
public:
	EffectObj() {}
	void Init(ID3D11Device* device);
	void Update();
	void Render();

	enum class TYPE
	{
		THUNDER,		//雷
		BLACKMIST,	//黒もや
		TST,					//テスト
		CHARGE,			//溜め
		SLASHING,		//斬撃的な
		RUN,
		BRIBIRI,
		CIRCLE,		//魔法陣
		PLAYERCHARGE, 
		SUKILL,		 //スキル
		HEAL,           //回復
		MPMAX,
		END
	};
	void Play(TYPE i);
	void Plays(float elapsedTime, TYPE i, int num, DirectX::XMFLOAT3 h, float time);//種類、出す数、間隔、時間の間隔
	void Stop(TYPE i);

	bool Exist(TYPE i)
	{
		return manager->Exists(handle[static_cast<int>(i)]);
	}
	//セッター
	void SetPosition(TYPE i, const DirectX::XMFLOAT3& p)
	{
		pos[static_cast<int>(i)] = p;
		//this->p = p;
	}
	void SetScale(TYPE i, const DirectX::XMFLOAT3& s)
	{
		scale[static_cast<int>(i)] = s;
		//this->s = s;
	}
	void SetAngle(TYPE i, const DirectX::XMFLOAT3& a)
	{
		angle[static_cast<int>(i)].x = (a.x);
		angle[static_cast<int>(i)].y = (a.y);
		angle[static_cast<int>(i)].z = (a.z);
	}
	void SetColor(TYPE i, const DirectX::XMFLOAT4& c)
	{
		color[static_cast<int>(i)].R = static_cast<uint8_t>(c.x * 255.f);
		color[static_cast<int>(i)].G = static_cast<uint8_t>(c.y * 255.f);
		color[static_cast<int>(i)].B = static_cast<uint8_t>(c.z * 255.f);
		color[static_cast<int>(i)].A = static_cast<uint8_t>(c.w * 255.f);
	}
	void SetAccel(TYPE i, const DirectX::XMFLOAT3& a) { accel[static_cast<int>(i)] = a; }

	void SetStopFlag(TYPE i, bool s) { stopFlag[static_cast<int>(i)] = s; }

	void SetStraightFlag(bool s) { straightFlag = s; }

	//ゲッター
	const DirectX::XMFLOAT3& GetPosition(TYPE i)const { return pos[static_cast<int>(i)]; }
	const DirectX::XMFLOAT3& GetScale(TYPE i)const { return scale[static_cast<int>(i)]; }
	const DirectX::XMFLOAT3& GetAngle(TYPE i)const { return angle[static_cast<int>(i)]; }
	const DirectX::XMFLOAT3& GetAccel(TYPE i)const { return accel[static_cast<int>(i)]; }
	const Effekseer::Color& GetColor(TYPE i)const { return color[static_cast<int>(i)]; }
	const bool& GetStopFlag(TYPE i)const { return stopFlag[static_cast<int>(i)]; }
	const bool& GetStraightFlag()const { return straightFlag; }
	EffectObj::~EffectObj()
	{
		// エフェクトの解放
		for (int i = 0; i < num; i++)
		{
			ES_SAFE_RELEASE(effect[i]);
		}
		renderer->Destroy();
		//マージャーの破棄
		manager->Destroy();
	}
private:
	int count = 0;
	float time = 0;
	bool straightFlag = true;//連続で出すとき用
	static inline  EffectObj* instance;
	::EffekseerRendererDX11::Renderer* renderer;
	::Effekseer::Manager* manager;

	static const int num = static_cast<int>(TYPE::END);
	::Effekseer::Effect* effect[num];
	Effekseer::Handle handle[num];
	//DirectX::XMFLOAT3	p;
	//DirectX::XMFLOAT3	s;

	DirectX::XMFLOAT3	pos[num];
	DirectX::XMFLOAT3	speed[num];
	DirectX::XMFLOAT3	accel[num];
	DirectX::XMFLOAT3	angle[num];
	DirectX::XMFLOAT3	scale[num];
	//DirectX::XMFLOAT4	c[num];
	Effekseer::Color color[num];
	bool stopFlag[num];
	float w;


	bool exist;
	int  dispTimer;
	int  dispMaxTimer;

public:
	static void Create()
	{
		if (instance != nullptr) return;
		instance = new EffectObj;
	}
	static EffectObj& GetInstance()
	{
		return *instance;
	}
	static void Destory()
	{
		if (instance != nullptr)
		{
			delete instance;
			instance = nullptr;
		}
	}
};
