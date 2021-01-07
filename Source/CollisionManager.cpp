#include "CollisionManager.h"
#include <DirectXMath.h>
#include "Particle.h"
#include "Fadeout.h"
void CollisionManager::CameraStageJudge(Stage& stage, PlayerObj& player)
{
	////レイピック
	std::shared_ptr<Character> hitleft = stage.GetHitLeft();
	DirectX::XMFLOAT3 position = player.GetPosition();
	DirectX::XMFLOAT3 front = player.GetFront();
	float dx = front.x * 10.f;
	float dz = front.z * 10.f;
	DirectX::XMFLOAT3 rayStart = DirectX::XMFLOAT3(position.x + dx, position.y, position.z + dz);	// レイを飛ばす開始位置
	DirectX::XMFLOAT3 rayEnd = DirectX::XMFLOAT3(position.x, position.y, position.z);	// レイを飛ばす終了位置
	DirectX::XMFLOAT3  outPosition;
	DirectX::XMFLOAT3  outNormal;

	if (-1 != stage.GetHitLeft()->RayPick(rayStart, rayEnd, &outPosition, &outNormal))
	{
		position.x = outPosition.x;
		position.z = outPosition.z;
		player.SetPosition(position);
	}
	//int materialIndex = Collision::MoveCheck(*hitleft,rayStart, rayEnd, &outPosition);
	player.SetPosition(position);

	//stage
	AABB left1 = stage.GetLeft1Cube();
	AABB left2 = stage.GetLeft2Cube();
	AABB right1 = stage.GetRight1Cube();
	AABB right2 = stage.GetRight2Cube();
	AABB front1 = stage.GetFront1Cube();
	AABB front2 = stage.GetFront2Cube();
	AABB haka1 = stage.GetHaka1Cube();
	AABB haka2 = stage.GetHaka2Cube();
	AABB back1 = stage.GetBack1Cube();
	AABB back2 = stage.GetBack2Cube();
	//camera
	AABB cameraCube = Camera::GetInstance().GetCube();
	//AABB playerCube;
	//playerCube.min = player.GetPosition();
	//playerCube.max = player.GetPosition();

	//float yLeng = (player.GetHitArea().max.y - player.GetHitArea().min.y);
	//playerCube.min.x -= player.GetHitArea().area;
	//playerCube.min.y -= yLeng;
	//playerCube.min.z -= player.GetHitArea().area;

	//playerCube.max.x += player.GetHitArea().area;
	//playerCube.max.y += yLeng;
	//playerCube.max.z += player.GetHitArea().area;
	//Cylinder playerArea = player.GetHitArea();

	////左
	//if (Collision::isHitAABB(left1, playerCube))
	//{
	//	player.SetPosition(DirectX::XMFLOAT3(left1.min.x - playerArea.area, player.GetPosition().y, player.GetPosition().z));
	//}
	//else if (Collision::isHitAABB(left2, playerCube))
	//{
	//	player.SetPosition(DirectX::XMFLOAT3(left2.min.x - playerArea.area, player.GetPosition().y, player.GetPosition().z));
	//}
	//if (Collision::isHitAABB(left1, playerCube))
	//{
	//	player.SetPosition(DirectX::XMFLOAT3(left1.min.x - playerArea.area, player.GetPosition().y, player.GetPosition().z));
	//}
	////右
	//if (Collision::isHitAABB(right1, playerCube))
	//{
	//	player.SetPosition(DirectX::XMFLOAT3(right1.max.x + playerArea.area, player.GetPosition().y, player.GetPosition().z));
	//}
	//else if (Collision::isHitAABB(right2, playerCube))
	//{
	//	player.SetPosition(DirectX::XMFLOAT3(right2.max.x + playerArea.area, player.GetPosition().y, player.GetPosition().z));
	//}
	////正面
	//if (Collision::isHitAABB(front1, playerCube))
	//{
	//	player.SetPosition(DirectX::XMFLOAT3(player.GetPosition().x, player.GetPosition().y, front1.max.z + playerArea.area));
	//}
	//else if (Collision::isHitAABB(front2, playerCube))
	//{
	//	player.SetPosition(DirectX::XMFLOAT3(player.GetPosition().x, player.GetPosition().y, front2.max.z + playerArea.area));
	//}
	//else if (Collision::isHitAABB(haka1, playerCube))
	//{
	//	player.SetPosition(DirectX::XMFLOAT3(player.GetPosition().x, player.GetPosition().y, haka1.max.z + playerArea.area));
	//}
	////後ろ
	//if (Collision::isHitAABB(back1, playerCube))
	//{
	//	player.SetPosition(DirectX::XMFLOAT3(player.GetPosition().x, player.GetPosition().y, back1.min.z - playerArea.area));
	//}
	//else 	if (Collision::isHitAABB(back2, playerCube))
	//{
	//	player.SetPosition(DirectX::XMFLOAT3(player.GetPosition().x, player.GetPosition().y, back2.min.z - playerArea.area));
	//}
	//else 	if (Collision::isHitAABB(haka2, playerCube))
	//{
	//	player.SetPosition(DirectX::XMFLOAT3(player.GetPosition().x, player.GetPosition().y, haka2.min.z - playerArea.area));

	//}
	//カメラ
	if (Collision::isHitAABB(left1, cameraCube))stage.GetLeftObj1()->SetExist(false);
	else stage.GetLeftObj1()->SetExist(true);
	if (Collision::isHitAABB(left2, cameraCube))stage.GetLeftObj2()->SetExist(false);
	else stage.GetLeftObj2()->SetExist(true);
	if (Collision::isHitAABB(right1, cameraCube))stage.GetRightObj1()->SetExist(false);
	else stage.GetRightObj1()->SetExist(true);
	if (Collision::isHitAABB(right2, cameraCube))stage.GetRightObj2()->SetExist(false);
	else stage.GetRightObj2()->SetExist(true);
	if (Collision::isHitAABB(front1, cameraCube))stage.GetFrontObj1()->SetExist(false);
	else stage.GetFrontObj1()->SetExist(true);
	if (Collision::isHitAABB(front2, cameraCube))stage.GetFrontObj2()->SetExist(false);
	else stage.GetFrontObj2()->SetExist(true);
	if (Collision::isHitAABB(haka1, cameraCube))stage.GetHaka1()->SetExist(false);
	else stage.GetHaka1()->SetExist(true);
	if (Collision::isHitAABB(back1, cameraCube))stage.GetBackObj1()->SetExist(false);
	else stage.GetBackObj1()->SetExist(true);
	if (Collision::isHitAABB(back2, cameraCube))stage.GetBackObj2()->SetExist(false);
	else stage.GetBackObj2()->SetExist(true);
	if (Collision::isHitAABB(haka2, cameraCube))stage.GetHaka2()->SetExist(false);
	else stage.GetHaka2()->SetExist(true);

}
void CollisionManager::Judge(float elapsed_time, PlayerObj& player, EnemyObj& enemy, Obj3D& stage)
{
	////レイピック
	//DirectX::XMFLOAT3 position = player.GetPosition();
	//DirectX::XMFLOAT3 front = player.GetFront();
	//DirectX::XMFLOAT3 rayStart = DirectX::XMFLOAT3(position.x, position.y, position.z);	// レイを飛ばす開始位置
	//DirectX::XMFLOAT3 rayEnd = DirectX::XMFLOAT3(position.x, position.y, position.z);	// レイを飛ばす終了位置
	//DirectX::XMFLOAT3  outPosition;	// レイの当たった位置
	//DirectX::XMFLOAT3  outNormal;	// レイの当たった面の法線


	//if (-1 != stage.RayPick(rayStart, rayEnd, &outPosition, &outNormal) && !player.GetHitFlag())
	//{
	//	position.y = outPosition.y;
	//	player.SetPosition(position);
	//}

	////レイピック
	//DirectX::XMFLOAT3 enmposition = enemy.GetPosition();
	//rayStart = DirectX::XMFLOAT3(enmposition.x, enmposition.y + 10.0f, enmposition.z);
	//rayEnd = DirectX::XMFLOAT3(enmposition.x, enmposition.y - 30.0f, enmposition.z);
	//if (-1 != stage.RayPick(rayStart, rayEnd, &outPosition, &outNormal))
	//{
	//	enmposition.y = outPosition.y;
	//	enemy.SetPosition(enmposition);
	//}

	Cylinder playerArea = player.GetHitArea();
	Cylinder enemyArea = enemy.GetHitArea();

	//playerとenemyの当たり判定(体同士の)
	if (Collision::isHitCylinder(playerArea, enemyArea) && !player.GetKnockBackFlag())
	{
		//player->SetHitFlag(true);
		player.SetHitPosition(enemy.GetPosition());

		//bossEnemy->SetHitFlag(true);
		enemy.SetHitPosition(player.GetPosition());

		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&enemy.GetPosition());
		DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&player.GetPosition());

		//ボスからプレイヤーへのベクトル
		DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(p, b);
		vec = DirectX::XMVector3Normalize(vec);

		DirectX::XMFLOAT3 angle;
		DirectX::XMStoreFloat3(&angle, vec);

		//半径の合計
		float len = player.GetHitArea().area + enemy.GetHitArea().area;

		player.SetPosition(DirectX::XMFLOAT3(enemy.GetPosition().x + angle.x * len, player.GetPosition().y, enemy.GetPosition().z + angle.z * len));
		player.SetHitFlag(true);
		enemy.SetHitFlag(true);
	}

	//playerの攻撃時
	if (player.GetAttackFlag() && !player.GetDamageFlag())
	{
		//playerの攻撃範囲とボスとの当たり判定
		Sphere playerAttackArea = player.GetHitSphere();
		if (Collision::isHitCylinderSphere(enemyArea, playerAttackArea))
		{
			player.stopFlag = true;
			enemy.SetHitFlag(true);
			player.SetHitFlag(true);
			enemy.SetHitPosition(enemy.GetPosition());
			enemy.SetHp(enemy.GetHp() - player.GetPower());
			//player.SetAttackFlag(false);
			player.SetDamageFlag(true);
			for (int i = 0; i < 50; i++)
			{
				ParticleManager::getInstance()->Add_Board(std::make_shared<ParticleHit>(), ParticleManager::getInstance()->Fire1, DirectX::XMFLOAT3(enemy.GetHitPosition().x, enemy.GetHitPosition().y + 10, enemy.GetHitPosition().z));
			}

			////ノックバック
			//DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&player.GetPosition());
			//DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&enemy.GetPosition());

			//DirectX::XMVECTOR d =  DirectX::XMVectorSubtract(e,p);
			//DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(d);//正規化
			//DirectX::XMFLOAT3 direction;
			//DirectX::XMStoreFloat3(&direction,dir);

			//enemy.SetDirection(direction);
			/*enemy.SetKnockBackSpeed(30);
			enemy.SetKnockBackTime(player.GetEnmKnockBack());
			enemy.SetKnockBackFlag(true);*/
		}
	}
	//ボスエネミーの攻撃時
	if (enemy.GetAttackFlag() && !enemy.GetDamageFlag() && !player.GetAccelFlag())
	{
		//ボスの攻撃範囲とplayerとの当たり判定
		if (enemy.GetRAttackFlag())
		{
			if (Collision::isHitCylinder(playerArea, enemyArea) && !player.GetKnockBackFlag())
			{
				player.stopFlag = true;
				enemy.SetHitFlag(true);
				player.SetHitFlag(true);
				player.SetHitPosition(player.GetPosition());
				player.SetHp(player.GetHp() - enemy.GetPower());
				enemy.SetAttackFlag(false);
				enemy.SetDamageFlag(true);
				player.SetKnockBackFlag(true, 1);
			}
		}
		else if (enemy.GetThunderAttackFlag())
		{
			Cylinder thunderArea = enemy.GetThunderCylinder();
			if (Collision::isHitCylinder(playerArea, thunderArea) && !player.GetKnockBackFlag())
			{
				player.stopFlag = true;
				player.SetHitFlag(true);
				player.SetHitPosition(player.GetPosition());
				player.SetHp(player.GetHp() - enemy.GetPower());
				enemy.SetAttackFlag(false);
				enemy.SetDamageFlag(true);
				player.SetKnockBackFlag(true, 1);
			}
		}
		else
		{
			Sphere enemyAttackArea = enemy.GetHitSphere();
			if (Collision::isHitCylinderSphere(playerArea, enemyAttackArea))
			{
				player.stopFlag = true;
				enemy.SetHitFlag(true);
				player.SetHitFlag(true);
				player.SetHitPosition(player.GetPosition());
				player.SetHp(player.GetHp() - enemy.GetPower());
				enemy.SetAttackFlag(false);
				enemy.SetDamageFlag(true);
				if (enemy.GetHighAttackFlag())  player.SetKnockBackFlag(true, 1);
				else  player.SetKnockBackFlag(true, 0);
			}
		}
	}
}


void CollisionManager::TargetJudge(float elapsed_time, PlayerObj& p, std::vector<EnemyObj>& e)
{
	////ターゲットカメラ用当たり判定
	//Sphere playerArea = p.GetTargetHitSphere();
	//auto& enemies = e;
	//for (size_t i = 0; i < enemies.size(); ++i)
	//{
	//	if (Collision::isHitCylinderSphere(enemies[i].GetHitArea(), playerArea))
	//	{
	//		D



	//	}
	//	

	//}



}
