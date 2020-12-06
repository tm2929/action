#pragma once
#include"sprite.h"
#include<memory>
class FadeOut
{
public:
	static FadeOut& GetInctence()
	{
		static FadeOut fadeout;
		return fadeout;
	}
	void Load(ID3D11Device* device, const wchar_t* fileName);
	bool Update(float elapasdTime);
	void MoveStart();
	void Render(ID3D11DeviceContext* context);
private:
	FadeOut()
	{
		moveFlag = false;
		colorW = 0.f;
	}
	std::unique_ptr<Sprite>fade;
	bool moveFlag;
	float colorW;
};
#define pFadeOut (FadeOut::GetInctence())