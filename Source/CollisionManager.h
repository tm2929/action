#pragma once
#include"playerobj.h"
#include "EnemyObj.h"
#include "Collision.h"
#include "Camera.h"
#include "Stage.h"
#include<vector>
class CollisionManager
{
public:
	static void  StageJudge(Stage& stage, PlayerObj& player);
	static  void Judge(float elapsed_time, PlayerObj& player, EnemyObj& enemy, Stage& stage);
	void TargetJudge(float elapsed_time, PlayerObj& p, std::vector<EnemyObj>& e);

	static void PlayerStageJudge(PlayerObj& player,Stage& stage);
	static void CameraStageJudge(Stage& stage);
};
