#include "UI.h"
Ui::Ui(ID3D11Device* device, const wchar_t* filename)
{

	sprite = std::make_unique<Sprite>(device, filename);

	sizeX = 0;
	max = 0;
	now = 0;
}
void Ui::Update(float elapsedTime, float max, float now, float width)
{
	this->max = max;
	this->now = now;
	this->width = width;

	sizeX = now * width;

	if (sizeX <= 0)
	{
		sizeX = 0;
	}

}
void Ui::Render(ID3D11DeviceContext* context, DirectX::XMFLOAT2 pos, float sizeY, DirectX::XMFLOAT2 texPos, DirectX::XMFLOAT2 texSize, float angle, DirectX::XMFLOAT4 color)
{

	sprite->Render(context, pos.x, pos.y, sizeX, sizeY, texPos.x, texPos.y, texSize.x, texSize.y, angle, color.x, color.y, color.z, color.w);


}