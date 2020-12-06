#pragma once
#include<d3d11.h>

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;
	virtual int  Update(float) = 0;
	virtual void Render(float, ID3D11DeviceContext*) = 0;

	enum SceneName
	{
		TITLE = 1,
		GAME,
		OVER,
		CLEAR,
	};
};

