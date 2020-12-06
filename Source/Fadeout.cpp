#include "Fadeout.h"

void FadeOut::Load(ID3D11Device* device, const wchar_t* fileName)
{
	fade = std::make_unique<Sprite>(device, fileName);
}

bool FadeOut::Update(float elapasdTime)
{
	if (!moveFlag)return false;
	colorW += elapasdTime;
	if (colorW >= 1.f)
	{
		colorW = 0.f;
		moveFlag = false;
		return true;
	}
	return false;
}

void FadeOut::MoveStart()
{
	moveFlag = true;
}

void FadeOut::Render(ID3D11DeviceContext* context)
{
	if (!moveFlag)return;
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &viewport);

	fade->Render(context, 0, 0, viewport.Width, viewport.Height, 0, 0, 1024, 1024, 0, 0, 0, 0, colorW);
}
