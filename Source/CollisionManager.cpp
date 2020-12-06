#include "CollisionManager.h"
#include <DirectXMath.h>
#include "Particle.h"
#include "Fadeout.h"
void CollisionManager::Judge(float elapsed_time, PlayerObj& player, EnemyObj& enemy, Obj3D& stage)
{
	//���C�s�b�N
	DirectX::XMFLOAT3 position = player.GetPosition();
	DirectX::XMFLOAT3 rayStart = DirectX::XMFLOAT3(position.x, position.y, position.z);	// ���C���΂��J�n�ʒu
	DirectX::XMFLOAT3 rayEnd = DirectX::XMFLOAT3(position.x, position.y - 3.0f, position.z);	// ���C���΂��I���ʒu
	DirectX::XMFLOAT3  outPosition;	// ���C�̓��������ʒu
	DirectX::XMFLOAT3  outNormal;	// ���C�̓��������ʂ̖@��


	if (-1 != stage.RayPick(rayStart, rayEnd, &outPosition, &outNormal) && !player.GetHitFlag())
	{
		position.y = outPosition.y;
		player.SetPosition(position);
	}

	//���C�s�b�N
	DirectX::XMFLOAT3 enmposition = enemy.GetPosition();
	rayStart = DirectX::XMFLOAT3(enmposition.x, enmposition.y + 10.0f, enmposition.z);
	rayEnd = DirectX::XMFLOAT3(enmposition.x, enmposition.y - 30.0f, enmposition.z);
	if (-1 != stage.RayPick(rayStart, rayEnd, &outPosition, &outNormal))
	{
		enmposition.y = outPosition.y;
		enemy.SetPosition(enmposition);
	}

	Cylinder playerArea = player.GetHitArea();
	Cylinder enemyArea = enemy.GetHitArea();

	//player��enemy�̓����蔻��(�̓��m��)
	if (Collision::isHitCylinder(playerArea, enemyArea) && !player.GetKnockBackFlag())
	{
		//player->SetHitFlag(true);
		player.SetHitPosition(enemy.GetPosition());

		//bossEnemy->SetHitFlag(true);
		enemy.SetHitPosition(player.GetPosition());

		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&enemy.GetPosition());
		DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&player.GetPosition());

		//�{�X����v���C���[�ւ̃x�N�g��
		DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(p, b);
		vec = DirectX::XMVector3Normalize(vec);

		DirectX::XMFLOAT3 angle;
		DirectX::XMStoreFloat3(&angle, vec);

		//���a�̍��v
		float len = player.GetHitArea().area + enemy.GetHitArea().area;

		player.SetPosition(DirectX::XMFLOAT3(enemy.GetPosition().x + angle.x * len, player.GetPosition().y, enemy.GetPosition().z + angle.z * len));
		player.SetHitFlag(true);
		enemy.SetHitFlag(true);
	}

	//player�̍U����
	if (player.GetAttackFlag() && !player.GetDamageFlag())
	{
		//player�̍U���͈͂ƃ{�X�Ƃ̓����蔻��
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

			////�m�b�N�o�b�N
			//DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&player.GetPosition());
			//DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&enemy.GetPosition());

			//DirectX::XMVECTOR d =  DirectX::XMVectorSubtract(e,p);
			//DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(d);//���K��
			//DirectX::XMFLOAT3 direction;
			//DirectX::XMStoreFloat3(&direction,dir);

			//enemy.SetDirection(direction);
			/*enemy.SetKnockBackSpeed(30);
			enemy.SetKnockBackTime(player.GetEnmKnockBack());
			enemy.SetKnockBackFlag(true);*/
		}
	}
	//�{�X�G�l�~�[�̍U����
	if (enemy.GetAttackFlag() && !enemy.GetDamageFlag() && !player.GetAccelFlag())
	{
		//�{�X�̍U���͈͂�player�Ƃ̓����蔻��
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
	////�^�[�Q�b�g�J�����p�����蔻��
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
