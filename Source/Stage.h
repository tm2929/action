#pragma once
#include "Obj3D.h"
class Stage
{
public:
	Stage(ID3D11Device* device);
	void Editor();
	void Update();
private:
	struct Data
	{
		DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 0, 0);
		DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1, 1, 1);
		DirectX::XMFLOAT3 angle = DirectX::XMFLOAT3(0, 0, 0);
	};
	void Load();
	void Save();
	std::shared_ptr<Obj3D>floor;
	std::shared_ptr<Obj3D>wall[4];
};