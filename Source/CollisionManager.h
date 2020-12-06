#pragma once
#include"playerobj.h"
#include "EnemyObj.h"
#include "Collision.h"
#include<vector>
class CollisionManager
{
public:
	static  void Judge(float elapsed_time, PlayerObj& player, EnemyObj& enemy, Obj3D& stage);
	void TargetJudge(float elapsed_time, PlayerObj& p, std::vector<EnemyObj>& e);

};
