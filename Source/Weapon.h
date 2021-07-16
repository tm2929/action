#pragma once
#include "Character.h"
class Weapon : public Character
{
private:
	int weaponPos = 47; //プレイヤーボーン番号(47 = weapon_r)
public:
	Weapon(std::shared_ptr<ModelResource>resource);
	void Update(float elapsedTime, std::shared_ptr<Obj3D> o);



};