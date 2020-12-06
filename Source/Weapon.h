#pragma once
#include "Character.h"
class Weapon : public Character
{
public:
	Weapon(std::shared_ptr<ModelResource>resource);
	void Update(float elapsedTime, std::shared_ptr<Obj3D> o);



};