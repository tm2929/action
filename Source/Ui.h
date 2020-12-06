#pragma once
#include "sprite.h"
#include<memory>


class Ui
{
public:
	void SetMax(const float max) { this->max = max; }
	void SetNow(const float noe) { this->now = now; }

	Ui(ID3D11Device* device, const wchar_t*);
	void Update(float elapsedTime, float max, float now, float width);
	void Render(ID3D11DeviceContext* context, DirectX::XMFLOAT2 pos, float sizeY, DirectX::XMFLOAT2 texPos, DirectX::XMFLOAT2 texSize, float angle, DirectX::XMFLOAT4 color);
private:
	std::unique_ptr<Sprite> sprite;

	float max;
	float now;
	float width;
	float sizeX;
};