//#include "NormalEnemy.h"
//
//NormalEnemy::NormalEnemy(std::shared_ptr<ModelResource> resource)
//{
//	enemyObj = std::make_unique<EnemyObj>(resource);
//
//	//初期設定
//	float z = rand() % 50;
//	enemyObj->SetPosition(DirectX::XMFLOAT3(0.0f, 30.0f, z));
//	enemyObj->SetScale(DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f));
//	enemyObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
//
//	//ノックバック
//	enemyObj->SetKnockBackFlag(false);
//
//	//HP
//	enemyObj->SetHp(50.f);
//	enemyObj->SetMaxHp(50.f);
//
//	animNo = 0;
//	enemyObj->GetModel()->PlayAnimation(animNo, true);
//
//	hitData.Init();
//
//	enemyObj->SetHitAreaLeng(hitData.hitLength);
//	enemyObj->CalculateTransform();
//	enemyObj->HitAreaTransform();
//	enemyObj->SetHitAttackAreaLeng(hitData.hitAttackLength);
//	enemyObj->BoonNameInput();
//
//	color[0] = DirectX::XMFLOAT4(1, 1, 1, 1);
//	color[1] = DirectX::XMFLOAT4(1, 1, 1, 1);//当たってない
//	color[2] = DirectX::XMFLOAT4(1, 0, 0, 1);//当たった
//
//	state = STATE::WAIT;
//}
//
//void NormalEnemy::Update(float elapsedTime)
//{
//
//}
//
//void NormalEnemy::Imgui()
//{
//}
//
//void NormalEnemy::SetWaitState()
//{
//}
//
//void NormalEnemy::UpdateWaitState(float elapsedTime)
//{
//}
//
//void NormalEnemy::SetRunState()
//{
//}
//
//void NormalEnemy::UpdateRunState(float elapsedTime)
//{
//}
//
//void NormalEnemy::SetAttackState()
//{
//}
//
//void NormalEnemy::UpdateAttackState(float elapsedTime)
//{
//}
//
//void NormalEnemy::SetKnockBackState()
//{
//}
//
//void NormalEnemy::UpdateKnockBackState(float elapsedTime)
//{
//}
