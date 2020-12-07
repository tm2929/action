#include "Enemy.h"
#include "HitAreaRnder.h"
#include "Camera.h"
#include <memory>
#include <DirectXMath.h>
#include "EffectManager.h"
#include "AttackLine.h"
#include "Math.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif
#include "Easing.h"
#include "Fadeout.h"
Enemy::Enemy(std::shared_ptr<ModelResource> resource)
{
	enemyObj = std::make_unique<EnemyObj>(resource);
	//�����ݒ�
	enemyObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -200.0f));
	enemyObj->SetScale(DirectX::XMFLOAT3(.2f, 0.2f, 0.2f));
	enemyObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	angle = enemyObj->GetAngle();
	pos = enemyObj->GetPosition();
	//�m�b�N�o�b�N
	enemyObj->SetKnockBackFlag(false);
	//�V���b�g
	shotAngle = 0.f;
	shotTime = 0.f;

	//HP
	enemyObj->SetHp(100.f);
	enemyObj->SetMaxHp(100.f);

	animNo = 0;
	enemyObj->GetModel()->PlayAnimation(animNo, true);
	animSpeed = 1.0f;
	animTime = 0.f;
	animStopFlag = false;

	hitData.Init();
	choiceData.Init();
	attackData.Init();
	speedData.Init();
	thunderData.Init();

	enemyObj->SetHitAreaLeng(hitData.hitLength);
	enemyObj->CalculateTransform();
	enemyObj->HitAreaTransform();
	enemyObj->SetHitAttackAreaLeng(hitData.hitAttackLength);
	enemyObj->BoneNameInput();

	enemyObj->SetAccelSpeed(speedData.accelSpeed);
	enemyObj->SetRunSpeed(speedData.runSpeed);

	color[0] = DirectX::XMFLOAT4(1, 1, 1, 1);
	color[1] = DirectX::XMFLOAT4(1, 1, 1, 1);//�������ĂȂ�
	color[2] = DirectX::XMFLOAT4(1, 0, 0, 1);//��������
	//state
	actionState = ACTION::FIRST;
	state = STATE::WAIT;
	waiteTime = 0.f;
	len = 0;
	dot = 0;
	cross = DirectX::XMFLOAT3(0, 0, 0);
	attackMoveFlag = false;
	number = 0;
	//�U����
	power = hitData.damage;
	//LoadData();
	enemyObj->SetRAttackFlag(false);
	enemyObj->SetHighAttackFlag(false);

	enemyObj->GetModel()->SetBlendTime(5.0f);

	attackStartFlag = false;
}
void Enemy::Update(float elapsedTime)
{
	if (enemyObj->GetHp() <= 0) 	enemyObj->SetExist(false);
	if (!enemyObj->GetExist() && state != STATE::DEATH)
	{
		SetDeathState();
	}
	//�A�j���[�V�����X�V
	enemyObj->AnimUpdate(elapsedTime * animSpeed);
	//�A�j���[�V�������Ԏ擾
	animTime = enemyObj->GetModel()->GetAnimationSeconds();

	if (enemyObj->GetRAttackFlag())
	{
		static float Rscale;
		static float dispTimer = 0;
		dispTimer += elapsedTime;
		Rscale = OutSine(dispTimer, 0.5f, 30.f, hitData.hitLength);
		enemyObj->SetHitAreaLeng(Rscale);
		if (dispTimer >= 0.5f)
		{
			dispTimer = 0;
		}
	}
	else
	{
		enemyObj->SetHitAreaLeng(hitData.hitLength);
	}

	enemyObj->CalculateTransform();
	enemyObj->HitAttackTransform();
	//�����蔻��X�V
	enemyObj->HitAreaTransform();
	enemyObj->SetFront(DirectX::XMFLOAT3(sinf(angle.y), 0, cosf(angle.y)));
	//�J�����^�[�Q�b�g����ɃG�l�~�[���Z�b�g
	Camera::GetInstance().SetEnemy(DirectX::XMFLOAT3(enemyObj->GetPosition().x, enemyObj->GetHeadPosition().y, enemyObj->GetPosition().z));

	//thunder�����蔻��
	enemyObj->SetThunderAttackFlag(thunderData.exist);
	if (thunderData.exist)
	{
		thunderData.cylinder.HitAreaTransform(20, playerPos, thunderData.area);
		enemyObj->SetThunderCylinder(thunderData.cylinder);
	}
	enemyObj->SetPower(power);
	//�����蔻��\��
	if (enemyObj->GetHitFlag())
	{
		color[0] = color[2];
	}
	else
	{
		color[0] = color[1];
	}
	//�������蔻��
	pHitAreaRender.SetHitCylinder(enemyObj->GetThunderCylinder().min, enemyObj->GetThunderCylinder().max, enemyObj->GetThunderCylinder().area, color[0]);
	//�̓����蔻��
	pHitAreaRender.SetHitCylinder(enemyObj->GetHitArea().min, enemyObj->GetHitArea().max, enemyObj->GetHitArea().area, color[0]);
	enemyObj->SetHitFlag(false);
	//�U���p�����蔻��
	pHitAreaRender.SetHitSphere(enemyObj->GetHitSphere().position, enemyObj->GetHitSphere().area, color[0]);
	EffectObj::GetInstance().Plays(elapsedTime, EffectObj::TYPE::SLASHING, 6, DirectX::XMFLOAT3(sinf(enemyObj->GetAngle().y) * 4, 0, cosf(enemyObj->GetAngle().y) * 4), 0.1f);
	switch (state)
	{
	case STATE::FIRST:
		UpdateFirstState(elapsedTime);
		break;
	case STATE::CHOICE:
		UpdateChoiceState(elapsedTime);
		break;
	case STATE::WAIT:
		UpdateWaitState(elapsedTime);
		break;
	case STATE::SIDE:
		UpdatSideStepState(elapsedTime);
		break;
	case STATE::RUN:
		UpdateRunState(elapsedTime);
		break;
	case STATE::BACK:
		UpdateBackStepState(elapsedTime);
		break;
	case STATE::SHORTATTACK:
		UpdateShortAttackState(elapsedTime);
		break;
	case STATE::LONGATTACK:
		UpdateLongAttackState(elapsedTime);
		break;
	case STATE::KNOCKBACK:
		UpdateKnockBackState(elapsedTime);
		break;
		/*case STATE::TELEPORTATION:
			UpdateTeleportationState(elapsedTime);
			break;*/
	case STATE::DEATH:
		UpdateDeathState(elapsedTime);
		break;
	}
	if (state != STATE::SHORTATTACK && state != STATE::LONGATTACK)
	{
		enemyObj->SetAttackFlag(false);
	}
	if (animStopFlag)
	{
		animSpeed = 0;
	}
	//��false �H������u�Ԃ���true�ɂȂ�悤��
	enemyObj->SetHitFlag(false);
	Imgui();
}
void Enemy::SaveData()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/enmdata.bin", "wb");
	fwrite(&choiceData, sizeof(EnemyChoiceData), 1, fp);
	fclose(fp);
}
void Enemy::LoadData()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/enmdata.bin", "rb") == 0)
	{
		fread(&choiceData, sizeof(EnemyChoiceData), 1, fp);
		fclose(fp);
	}
}

void Enemy::Imgui()
{
#ifdef USE_IMGUI
	ImGui::Begin("enemy");
	ImGui::Text("hp%.f", enemyObj->GetHp());
	ImGui::Text("dot%f", dot);
	ImGui::Text("cross_y%f", cross.y);
	ImGui::Text("angle_y%f", enemyObj->GetAngle().y);
	//���x
	if (ImGui::CollapsingHeader(u8"���x"))
	{
		ImGui::Text("knockBackTime%f", enemyObj->GetKnockBackTime());
		ImGui::InputFloat("runSpeed", &speedData.runSpeed, 1.f);
		ImGui::InputFloat("accelSpeed", &speedData.accelSpeed, 1.f);
	}
	//TIME
	if (ImGui::CollapsingHeader("TIME"))
	{
		ImGui::Text("time%2f", waiteTime);
		ImGui::SliderFloat("waiteTime", &choiceData.waiteTime, 0, 50);
		ImGui::SliderFloat(u8"�ړ�time", &choiceData.time, 0, 1);
		ImGui::InputFloat("runTime", &choiceData.time, 0.1f);
		ImGui::SliderFloat("angleTime", &attackData.angleTime, 0, 1);
		ImGui::SliderFloat("rushTime", &attackData.rushTime, 0, 1);
		ImGui::InputFloat("chargeTime", &attackData.chargeTime, 1.0f);
	}
	//THUNDER
	if (ImGui::CollapsingHeader("THUNDER"))
	{
		ImGui::Text("exist %d", thunderData.exist);
		ImGui::InputFloat("thunderAttackStart", &thunderData.attackStartTime, 0.1f);
		ImGui::InputFloat("thunderAttackEnd", &thunderData.attackEndTime, 0.1f);
		ImGui::InputFloat("thunderEnd", &thunderData.endTime, 0.1f);
	}
	//STATE
	if (ImGui::CollapsingHeader("STATE"))
	{
		ImGui::Text("state%d", state);
		ImGui::Text(u8"����%.f", len);
		ImGui::Text("num%d", number);
		ImGui::InputInt("num", &choiceData.num, 0, 10);
		ImGui::SliderFloat("choiceLen[0]", &choiceData.choiceLen[0], 0, 50);
		ImGui::SliderFloat("choiceLen[1]", &choiceData.choiceLen[1], 0, 50);
		ImGui::SliderFloat("choiceLen[2]", &choiceData.choiceLen[2], 30, 150);
		//state�؂�ւ�
		if (ImGui::Button(u8"state�ύXFlag"))
		{
			debugFlag = !debugFlag;
		}
		if (debugFlag)
		{
			if (ImGui::CollapsingHeader(u8"state�ύX"))
			{
				if (ImGui::Button("ChoiceState"))
				{
					SetChoiceState();
				}
				if (ImGui::Button("ShortState"))
				{
					number = choiceData.num;
					SetShortAttackState();
				}
				if (ImGui::Button("LongState"))
				{
					number = choiceData.num;
					SetLongAttackState();
				}
				if (ImGui::Button("Run"))
				{
					SetRunState();
				}
				if (ImGui::Button("Back"))
				{
					SetBackStepState();
				}
				if (ImGui::Button("Side"))
				{
					SetSideStepState();
				}
			}
		}
		if (ImGui::Button("firstFlag"))
		{
			if (choiceData.firstFlag)choiceData.firstFlag = false;
			else choiceData.firstFlag = true;
		}
		ImGui::Text("firstFlag%d", choiceData.firstFlag);
		ImGui::SliderFloat("firstArea", &choiceData.firstArea, 0, 100);
		if (ImGui::Button("Save"))
		{
			SaveData();
		}
		if (ImGui::Button("Load"))
		{
			LoadData();
		}
	}
	//�ʒu
	if (ImGui::CollapsingHeader(u8"�ʒu"))
	{
		static DirectX::XMFLOAT3 pos;
		ImGui::SliderFloat("pos.x", &pos.x, -200, 200.f);
		ImGui::SliderFloat("pos.y", &pos.y, -200, 200.f);
		ImGui::SliderFloat("pos.z", &pos.z, -500, 200.f);
		enemyObj->SetPosition(pos);
	}
	//�����蔻��
	if (ImGui::CollapsingHeader(u8"�����蔻��"))
	{
		ImGui::Text("attackFlag%d", enemyObj->GetAttackFlag());
		ImGui::Text("damageFlag%d", enemyObj->GetDamageFlag());

		ImGui::SliderFloat(u8"�����蔻��T�C�Y", &hitData.hitLength, 0, 50.f);
		enemyObj->SetHitAreaLeng(hitData.hitLength);

		ImGui::SliderFloat(u8"�U�������蔻��T�C�Y", &hitData.hitAttackLength, 0, 50.f);
		enemyObj->SetHitAttackAreaLeng(hitData.hitAttackLength);

		ImGui::SliderFloat(u8"�������蔻��T�C�Y", &thunderData.area, 0, 50.f);

		ImGui::Text("hitSphere.pos.x_%f", enemyObj->GetHitSphere().position.x);
		ImGui::Text("hitSphere.pos.z_%f", enemyObj->GetHitSphere().position.z);
	}
	//Anime
	if (ImGui::CollapsingHeader("Anime"))
	{
		ImGui::SliderFloat("animSpeed", &animSpeed, 0, 2);
		ImGui::SliderFloat("animTime", &animTime, 0, 5);
		if (ImGui::Button("animStop"))
		{
			if (animStopFlag)
			{
				animStopFlag = false;
			}
			else
			{
				animStopFlag = true;
			}
		}
		if (ImGui::SliderInt("animation", &animNo, 0, enemyObj->GetModel()->GetResource()->GetAnimations().size() - 1))
		{
			enemyObj->SetAnim(animNo, true);
		}
	}
	enemyObj->AttackBoneNameEditor();
	ImGui::End();
#endif
}
void Enemy::SetFirstState()
{
	state = STATE::FIRST;
	animNo = static_cast<int>(ANIM::DAMAGE2);
	enemyObj->SetAnim(animNo, false);

	waiteTime = 0;
	actionState = ACTION::FIRST;

}
void Enemy::UpdateFirstState(float elapsedTime)
{
	switch (actionState)
	{
	case ACTION::FIRST:
		CalculateLength();
		//��苗���ɓ�������N���オ��i�퓬�J�n�j
		if (len < choiceData.firstArea && animNo != static_cast<int>(ANIM::DAMAGE3))
		{
			animNo = static_cast<int>(ANIM::DAMAGE3);
			enemyObj->SetAnim(animNo, false);
			actionState = ACTION::SECOND;
			attackStartFlag = true;
			EffectObj::GetInstance().SetScale(EffectObj::TYPE::BLACKMIST, DirectX::XMFLOAT3(10, 40, 10));
			EffectObj::GetInstance().SetColor(EffectObj::TYPE::BLACKMIST, DirectX::XMFLOAT4(1, 1, 1, 0.2f));
			EffectObj::GetInstance().Play(EffectObj::TYPE::BLACKMIST);
		}
		break;
	case ACTION::SECOND:
		attackStartFlag = false;
		if (!enemyObj->GetAnimContinue())
		{
			waiteTime += elapsedTime;

			if (waiteTime > 0.4f)
			{
				SetChoiceState();
				choiceData.firstFlag = false;//��x��������Ȃ��悤��
			}
		}
		break;
	}
}

void Enemy::SetDeathState()
{
	animNo = static_cast<int>(ANIM::DAMAGE1);
	state = STATE::DEATH;
	enemyObj->SetAnim(animNo, false);
	waiteTime = 0;
	animSpeed = 0.4f;
}

void Enemy::UpdateDeathState(float elapsedTime)
{
	if (animTime > 0.5f)
	{
		pFadeOut.MoveStart();
	}
}

void Enemy::SetChoiceState()
{
	animNo = static_cast<int>(ANIM::WAIT);
	state = STATE::CHOICE;
	enemyObj->SetAnim(animNo, true);

	len = 0;
	waiteTime = 0;
}

void Enemy::UpdateChoiceState(float elapsedTime)
{
	if (debugFlag)
	{

	}
	else
	{
#if 0
		CalculateLength();
		if (!attackMoveFlag)
		{
			//�ߋ���
			if (len < choiceData.choiceLen[0])
			{
				number = IntRandom(0, 2);//number
				SetShortAttackState();
			}
			//������
			else /*if(len < choiceData.choiceLen[1])*/
			{
				number = IntRandom(0, 2);
				if (number == 0)
				{
					SetRunState();
				}
				else
				{
					number = 0;
					SetLongAttackState();
				}
			}

		}
		else if (beforeNumber == SHORTATTACK::UATTACK)
		{
			SetWaitState();
		}
		else
		{
			number = IntRandom(0, 20);
			if (number <= 5)
			{
				SetSideStepState();
			}
			else if (number <= 11)
			{
				SetBackStepState();
			}
			else if (number <= 17)
			{
				SetRunState();
			}
			else
			{
				number = /*SHORTATTACK::SATTACK;*/IntRandom(0, 2);//number
				SetShortAttackState();
				//SetSideStepState();
	}
}
		beforeNumber = number;
#else
		number = LONGATTACK::THUNDER;
		SetLongAttackState();
	}
#endif
}
//�ҋ@
void Enemy::SetWaitState()
{
	state = STATE::WAIT;
	animNo = static_cast<int>(ANIM::WAIT);
	enemyObj->SetAnim(animNo, true);
	waiteTime = 0;
	attackMoveFlag = false;
	animSpeed = 3.f;
}
void Enemy::UpdateWaitState(float elapsedTime)
{
	if (choiceData.firstFlag)
	{
		SetFirstState();
		animSpeed = 1.f;
	}
	if (enemyObj->GetKnockBackFlag())
	{
		SetKnockBackState();
	}
	waiteTime += elapsedTime;

	if (waiteTime > choiceData.waiteTime && !choiceData.firstFlag)
	{
		SetChoiceState();
		animSpeed = 1.f;
	}
}

void Enemy::SetSideStepState()
{
	state = STATE::SIDE;
	animNo = static_cast<int>(ANIM::RUN);
	enemyObj->SetAnim(animNo, true);
	enemyObj->SetAccelSpeed(speedData.runSpeed);
	waiteTime = 0;
	number = IntRandom(0, 1);
	pos = enemyObj->GetPosition();//�|�W�V����
	attackMoveFlag = false;
	angle = enemyObj->GetAngle();
}

void Enemy::UpdatSideStepState(float elapsedTime)
{
	CalculateDotCross();
	//�⊮�p
	waiteTime += elapsedTime;
	if (waiteTime < choiceData.time)
	{
		DirectX::XMFLOAT3 toPlayerAngle;
		DirectX::XMStoreFloat3(&toPlayerAngle, vec);
		//�����ύX
		if (acosf(dot) > DirectX::XMConvertToRadians(5))
		{
			if (cross.y < 0)
				angle.y -= DirectX::XMConvertToRadians(360) * elapsedTime;//acosf�ŃV�[�^�i�p�x�j���ł�@
			else
				angle.y += DirectX::XMConvertToRadians(360) * elapsedTime;
		}
		else
		{
			//DirectX::XMConvertToRadians(num)��菬�����Ȃ����ꍇ�Ԃ�Ȃ����
			angle.y = atan2f(toPlayerAngle.x, toPlayerAngle.z);
		}
		enemyObj->SetAngle(angle);

		DirectX::XMFLOAT3 right;
		DirectX::XMFLOAT3 n = DirectX::XMFLOAT3(sinf(angle.y), 0, cosf(angle.y));//�O�x�N�g��
		DirectX::XMVECTOR vn = DirectX::XMLoadFloat3(&n);
		DirectX::XMVECTOR up = { 0,1,0 };//��x�N�g��
		DirectX::XMVECTOR vec = DirectX::XMVector3Cross(vn, up);

		DirectX::XMStoreFloat3(&right, vec);
		if (number == 0)
		{
			pos.x += right.x * enemyObj->GetAccelSpeed() * elapsedTime;
			pos.z += right.z * enemyObj->GetAccelSpeed() * elapsedTime;
		}
		else
		{
			pos.x -= right.x * enemyObj->GetAccelSpeed() * elapsedTime;
			pos.z -= right.z * enemyObj->GetAccelSpeed() * elapsedTime;
		}
		if (waiteTime > choiceData.time * 0.5f)
		{
			enemyObj->SetAccelSpeed(30.f);
		}
		enemyObj->SetPosition(pos);
	}
	else
	{
		SetChoiceState();
	}
}
void Enemy::SetRunState()//�������ƃv���C���[�ɋߕt��
{
	state = STATE::RUN;
	animNo = static_cast<int>(ANIM::RUN);
	enemyObj->SetAnim(animNo, true);
	enemyObj->SetRunSpeed(speedData.runSpeed);
	waiteTime = 0;
	pos = enemyObj->GetPosition();
	attackMoveFlag = false;
}
void Enemy::UpdateRunState(float elapsedTime)
{
	CalculateLength();
	CalculateDotCross();
	waiteTime += elapsedTime;
	if (waiteTime < choiceData.time)
	{
		DirectX::XMFLOAT3 toPlayerAngle;
		DirectX::XMStoreFloat3(&toPlayerAngle, vec);
		//�����ύX

		if (acosf(dot) > DirectX::XMConvertToRadians(5))
		{
			if (cross.y < 0)
				angle.y -= DirectX::XMConvertToRadians(360) * elapsedTime;//acosf�ŃV�[�^�i�p�x�j���ł�@
			else
				angle.y += DirectX::XMConvertToRadians(360) * elapsedTime;
		}
		else
		{
			//DirectX::XMConvertToRadians(num)��菬�����Ȃ����ꍇ�Ԃ�Ȃ����
			angle.y = atan2f(toPlayerAngle.x, toPlayerAngle.z);
		}
		enemyObj->SetAngle(angle);
		//�ړ�
		if (len > 20) //player�Ƃ̋�����20���傫���ꍇ�̂�player�̂ق��ֈړ�
		{
			pos.x += sinf(angle.y) * enemyObj->GetRunSpeed() * elapsedTime;
			pos.z += cosf(angle.y) * enemyObj->GetRunSpeed() * elapsedTime;
			enemyObj->SetPosition(pos);
		}
		else
		{
			SetChoiceState();
		}
		if (waiteTime > choiceData.time * 0.5f)
		{
			enemyObj->SetRunSpeed(30.f);
		}
	}
	else
	{
		SetChoiceState();
	}
}

void Enemy::SetBackStepState()
{
	state = STATE::BACK;
	animNo = static_cast<int>(ANIM::RUN);
	enemyObj->SetAccelSpeed(speedData.runSpeed);
	enemyObj->SetAnim(animNo, true);
	pos = enemyObj->GetPosition();
	attackMoveFlag = false;
}

void Enemy::UpdateBackStepState(float elapsedTime)
{
	CalculateLength();
	CalculateDotCross();
	waiteTime += elapsedTime;
	if (waiteTime < choiceData.time)
	{
		DirectX::XMFLOAT3 toPlayerAngle;
		DirectX::XMStoreFloat3(&toPlayerAngle, vec);
		//�����ύX
		if (acosf(dot) > DirectX::XMConvertToRadians(5))
		{
			if (cross.y < 0) angle.y -= DirectX::XMConvertToRadians(180) * elapsedTime;//acosf�ŃV�[�^�i�p�x�j���ł�@
			else angle.y += DirectX::XMConvertToRadians(180) * elapsedTime;
		}
		else angle.y = atan2f(toPlayerAngle.x, toPlayerAngle.z);

		enemyObj->SetAngle(angle);
		//�ړ�
		pos.x -= sinf(angle.y) * enemyObj->GetAccelSpeed() * elapsedTime;
		pos.z -= cosf(angle.y) * enemyObj->GetAccelSpeed() * elapsedTime;
		enemyObj->SetPosition(pos);
	}
	else SetChoiceState();
}

void Enemy::SetShortAttackState()
{
	attackMoveFlag = true;//�U���X�e�[�g�ɓ�����
	if (number == SHORTATTACK::SATTACK)
	{
		animNo = static_cast<int>(ANIM::SATTACK);
		enemyObj->SetAnim(animNo, false);
		enemyObj->SetAccelSpeed(speedData.runSpeed);
		attackData.angleTime = 0.6f;
		attackData.chargeTime = 0.4f;

	}
	if (number == SHORTATTACK::RATTACK)
	{
		animNo = static_cast<int>(ANIM::RATTACK1);
		enemyObj->SetAnim(animNo, false);

		attackData.chargeTime = 1.0f;
	}
	if (number == SHORTATTACK::UATTACK)
	{
		//�`���[�W�G�t�F�N�g
		EffectObj::GetInstance().SetScale(EffectObj::TYPE::CHARGE, DirectX::XMFLOAT3(10, 10, 10));
		EffectObj::GetInstance().Play(EffectObj::TYPE::CHARGE);

		animNo = static_cast<int>(ANIM::UATTACK);
		enemyObj->SetAnim(animNo, false);
	}
	//������
	state = STATE::SHORTATTACK;
	waiteTime = 0;
	actionState = ACTION::FIRST;
	animSpeed = 1.0f;
	angle = enemyObj->GetAngle();
	enemyObj->SetDamageFlag(false);
	enemyObj->SetRAttackFlag(false);
	enemyObj->SetHighAttackFlag(false);
}

void Enemy::UpdateShortAttackState(float elapsedTime)
{
	EffectObj::GetInstance().SetPosition(EffectObj::TYPE::CHARGE, enemyObj->GetHitSphere().position);
	if (number == 0)
	{
		SAttack(elapsedTime);
	}
	if (number == 1)
	{
		RAttack(elapsedTime);
	}
	if (number == 2)
	{
		UAttack(elapsedTime);
	}
}

void Enemy::SetLongAttackState()
{
	attackMoveFlag = true;//�U���X�e�[�g�ɓ�����
	if (number == LONGATTACK::RUSH)
	{
		attackData.angleTime = 0.6f;
		attackData.rushFlag[1] = false;
		attackData.rushFlag[0] = false;
		animNo = static_cast<int>(ANIM::FATTACK1);
		enemyObj->SetAnim(animNo, false);
		enemyObj->SetAccelSpeed(speedData.accelSpeed);
	}
	if (number == LONGATTACK::THUNDER)
	{
		thunderData.exist = true;
		playerPos = Camera::GetInstance().GetTargetPos();//���o���ʒu
		playerPos.y = enemyObj->GetPosition().y;

		EffectObj::GetInstance().SetPosition(EffectObj::TYPE::BRIBIRI, DirectX::XMFLOAT3(playerPos));
		EffectObj::GetInstance().SetScale(EffectObj::TYPE::BRIBIRI, DirectX::XMFLOAT3(1, 0.3, 1));
		EffectObj::GetInstance().SetColor(EffectObj::TYPE::BRIBIRI, DirectX::XMFLOAT4(0.4f, 1.f, 1.f, 1.f));
		EffectObj::GetInstance().Play(EffectObj::TYPE::BRIBIRI);
		animNo = static_cast<int>(ANIM::HANDUP1);
		enemyObj->SetAnim(animNo, false);
		attackData.chargeTime = 0.7f;
	}
	state = STATE::LONGATTACK;
	waiteTime = 0;
	actionState = ACTION::FIRST;
	enemyObj->SetDamageFlag(false);
	enemyObj->SetHighAttackFlag(false);
}

void Enemy::UpdateLongAttackState(float elapsedTime)
{
	EffectObj::GetInstance().SetPosition(EffectObj::TYPE::CHARGE, enemyObj->GetHitSphere().position);
	EffectObj::GetInstance().SetPosition(EffectObj::TYPE::TST, DirectX::XMFLOAT3(enemyObj->GetPosition().x + sinf(enemyObj->GetAngle().y) * 50.f, 0, enemyObj->GetPosition().z + cosf(enemyObj->GetAngle().y) * 50.f));
	if (number == LONGATTACK::RUSH)
	{
		RushAttack(elapsedTime);
	}
	if (number == LONGATTACK::THUNDER)
	{
		ThunderAttack(elapsedTime);
	}
}
void Enemy::SetTeleportationState()
{
	animNo = static_cast<int>(ANIM::HANDUP1);
	enemyObj->SetAnim(animNo, false);
	actionState = ACTION::FIRST;
	state = STATE::TELEPORTATION;
}
void Enemy::UpdateTeleportationState(float elapsedTime)
{
	CalculateLength();
	CalculateDotCross();
	DirectX::XMFLOAT3 toPlayerAngle;
	DirectX::XMStoreFloat3(&toPlayerAngle, vec);
	switch (actionState)
	{
	case ACTION::FIRST:
		if (acosf(dot) > DirectX::XMConvertToRadians(5))
		{
			if (cross.y < 0)
				angle.y -= DirectX::XMConvertToRadians(180) * elapsedTime;//acosf�ŃV�[�^�i�p�x�j���ł�@
			else
				angle.y += DirectX::XMConvertToRadians(180) * elapsedTime;
		}
		else
		{
			//DirectX::XMConvertToRadians(num)��菬�����Ȃ����ꍇ�Ԃ�Ȃ����
			angle.y = atan2f(toPlayerAngle.x, toPlayerAngle.z);
		}
		enemyObj->SetAngle(angle);
		waiteTime += elapsedTime;
		if (waiteTime > 1.5f)
		{
			actionState = ACTION::SECOND;
			animNo = static_cast<int>(ANIM::HANDUP3);
			enemyObj->SetAnim(animNo, false);
		}
		break;
	case ACTION::SECOND:
		if (!enemyObj->GetAnimContinue())//�߂��wait��
		{
			actionState = ACTION::THIRD;
		}
		break;

	case ACTION::THIRD:
		pos.x = pos.x - sinf(angle.y) * 60;
		pos.z = pos.z - cosf(angle.y) * 60;

		enemyObj->SetPosition(pos);
		SetChoiceState();
		break;
	}
}
void Enemy::SAttack(float elapsedTime)//�Q�A��
{
	CalculateLength();
	CalculateDotCross();
	//�����ύX
	DirectX::XMFLOAT3 toPlayerAngle;
	DirectX::XMStoreFloat3(&toPlayerAngle, vec);
	switch (actionState)
	{
	case ACTION::FIRST:
		if (animTime > 0.8)
		{
			if (len > 20)
			{
				pos.x += sinf(angle.y) * enemyObj->GetAccelSpeed() * elapsedTime;
				pos.z += cosf(angle.y) * enemyObj->GetAccelSpeed() * elapsedTime;
				enemyObj->SetPosition(pos);
			}
		}
		else
		{
			if (acosf(dot) > DirectX::XMConvertToRadians(5))
			{
				if (cross.y < 0)
					angle.y -= DirectX::XMConvertToRadians(180) * elapsedTime;//acosf�ŃV�[�^�i�p�x�j���ł�@
				else
					angle.y += DirectX::XMConvertToRadians(180) * elapsedTime;
			}
			else
				angle.y = atan2f(toPlayerAngle.x, toPlayerAngle.z);
			enemyObj->SetAngle(angle);
		}
		if (animTime >= 1.0f)
		{
			actionState = ACTION::SECOND;
			enemyObj->SetAttackFlag(true);
		}
		break;
	case ACTION::SECOND:
		if (animTime > 1.55f)
		{
			enemyObj->SetAttackFlag(false);
		}
		else
		{
			pos.x += sinf(angle.y) * enemyObj->GetAccelSpeed() * elapsedTime;
			pos.z += cosf(angle.y) * enemyObj->GetAccelSpeed() * elapsedTime;
			enemyObj->SetPosition(pos);
		}
		if (!enemyObj->GetAnimContinue())
		{
			SetChoiceState();
		}
		break;
	}
}
void Enemy::RAttack(float elapsedTime)//��]�U��
{
	switch (actionState)
	{
	case ACTION::FIRST:
		waiteTime += elapsedTime;
		if (waiteTime > attackData.chargeTime)//���ߎ��ԁB���������]�U���ɓ���
		{
			animNo = static_cast<int>(ANIM::RATTACK2);
			enemyObj->SetAnim(animNo, false);
			actionState = ACTION::SECOND;
			waiteTime = 0;
			animSpeed = 2.0f;//�U�����x���グ��

			enemyObj->SetAttackFlag(true);
			enemyObj->SetRAttackFlag(true);
			enemyObj->SetHighAttackFlag(true);

		}
		break;
	case  ACTION::SECOND:
		if (!enemyObj->GetAnimContinue())//�I���Ό��ɖ߂郂�[�V������
		{
			animSpeed = 1.0f;
			animNo = static_cast<int>(ANIM::RATTACK3);
			enemyObj->SetAnim(animNo, false);
			actionState = ACTION::THIRD;
		}
		break;
	case ACTION::THIRD:
		if (!enemyObj->GetAnimContinue())//�߂��wait��
		{
			SetChoiceState();
			enemyObj->SetRAttackFlag(false);
			enemyObj->SetHighAttackFlag(false);

		}
	}
}
void Enemy::UAttack(float elapsedTime)//��U��U��
{
	CalculateLength();
	CalculateDotCross();
	//�⊮�p�ϐ�
	float a = angle.y;
	DirectX::XMVECTOR T = DirectX::XMVectorSet(0, 0, 0, 0);

	switch (actionState)
	{
	case ACTION::FIRST:
	{
		DirectX::XMFLOAT3 toPlayerAngle;
		DirectX::XMStoreFloat3(&toPlayerAngle, vec);
		//�����ύX
		if (acosf(dot) > DirectX::XMConvertToRadians(5))
		{
			if (cross.y < 0)
				angle.y -= DirectX::XMConvertToRadians(180) * elapsedTime;//acosf�ŃV�[�^�i�p�x�j���ł�@
			else
				angle.y += DirectX::XMConvertToRadians(180) * elapsedTime;
		}
		else
		{
			//DirectX::XMConvertToRadians(num)��菬�����Ȃ����ꍇ�Ԃ�Ȃ����
			angle.y = atan2f(toPlayerAngle.x, toPlayerAngle.z);
		}
		enemyObj->SetAngle(angle);
		waiteTime += elapsedTime;
		if (animTime > 0.2) // ���U���ɓ���Ȃ��悤�ɂ��߂����
		{
			animSpeed = 0;
			if (!AttackLine::GetInctance().GetExistFlag())
			{
				AttackLine::GetInctance().SetExistFlag(true);
			}
		}
		if (waiteTime > attackData.chargeTime)//���ߎ��ԁB��������U���ɓ���
		{
			actionState = ACTION::SECOND;
			waiteTime = 0;
			EffectObj::GetInstance().SetStraightFlag(true);
		}
		//attakLine
		AttackLine::GetInctance().SetScale(DirectX::XMFLOAT3(16.f, 0.1f, 60.f));
		AttackLine::GetInctance().SetAngle(enemyObj->GetAngle());
		AttackLine::GetInctance().SetPosition(DirectX::XMFLOAT3(enemyObj->GetPosition().x, 0.1f, enemyObj->GetPosition().z));
		//�a���G�t�F�N�g
		EffectObj::GetInstance().SetScale(EffectObj::TYPE::SLASHING, DirectX::XMFLOAT3(5, 5, 5));
		EffectObj::GetInstance().SetColor(EffectObj::TYPE::SLASHING, DirectX::XMFLOAT4(1, 1, 1, 1));
		EffectObj::GetInstance().SetPosition(EffectObj::TYPE::SLASHING, DirectX::XMFLOAT3(enemyObj->GetHitSphere().position.x, 0, enemyObj->GetHitSphere().position.z));
		EffectObj::GetInstance().SetAngle(EffectObj::TYPE::SLASHING, enemyObj->GetAngle());

	}
	break;
	case ACTION::SECOND:
		enemyObj->SetAttackFlag(true);
		enemyObj->SetHighAttackFlag(true);
		animSpeed = 1.0f;
		waiteTime += elapsedTime;
		if (waiteTime > 0.2f)
		{
			if (AttackLine::GetInctance().GetExistFlag())
			{
				AttackLine::GetInctance().SetExistFlag(false);
			}
		}
		if (!enemyObj->GetAnimContinue())//���[�V�������I�������ҋ@�ɖ߂�
		{
			waiteTime = 0;
			AttackLine::GetInctance().SetExistFlag(false);
			enemyObj->SetAttackFlag(false);
			enemyObj->SetHighAttackFlag(false);
			SetChoiceState();
		}
		break;
	}
}
void Enemy::RushAttack(float elapsedTime)//�ːi
{
	CalculateLength();
	CalculateDotCross();
	pos = enemyObj->GetPosition();
	DirectX::XMFLOAT3 toPlayerAngle;
	DirectX::XMStoreFloat3(&toPlayerAngle, vec);
	switch (actionState)
	{
	case ACTION::FIRST:
		waiteTime += elapsedTime;
		//�`���[�W����
		if (waiteTime > attackData.chargeTime)
		{
			actionState = ACTION::SECOND;
			waiteTime = 0;

			EffectObj::GetInstance().SetScale(EffectObj::TYPE::CHARGE, DirectX::XMFLOAT3(10, 10, 10));
			EffectObj::GetInstance().Play(EffectObj::TYPE::CHARGE);
		}
		break;
	case ACTION::SECOND: //��]
		waiteTime += elapsedTime;
		if (waiteTime < attackData.angleTime)
		{
			//�����ύX
			if (acosf(dot) > DirectX::XMConvertToRadians(5))
			{
				if (cross.y < 0)
					angle.y -= DirectX::XMConvertToRadians(180) * elapsedTime;//acosf�ŃV�[�^�i�p�x�j���ł�@
				else
					angle.y += DirectX::XMConvertToRadians(180) * elapsedTime;
			}
			else
			{
				//DirectX::XMConvertToRadians(num)��菬�����Ȃ����ꍇ�Ԃ�Ȃ����
				angle.y = atan2f(toPlayerAngle.x, toPlayerAngle.z);
			}
			enemyObj->SetAngle(angle);
		}
		else
		{
			actionState = ACTION::THIRD;
			waiteTime = 0;

			EffectObj::GetInstance().SetScale(EffectObj::TYPE::TST, DirectX::XMFLOAT3(5.f, 5.f, 5.f));
			EffectObj::GetInstance().SetColor(EffectObj::TYPE::TST, DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.7f));
			EffectObj::GetInstance().SetAngle(EffectObj::TYPE::TST, enemyObj->GetAngle());
			EffectObj::GetInstance().Play(EffectObj::TYPE::TST);
		}
		break;
	case ACTION::THIRD://�ːi
		waiteTime += elapsedTime;
		if (!attackData.rushFlag[0])
		{
			if (waiteTime < attackData.rushTime)
			{
				pos.x += sinf(angle.y) * enemyObj->GetAccelSpeed() * elapsedTime;
				pos.z += cosf(angle.y) * enemyObj->GetAccelSpeed() * elapsedTime;

				enemyObj->SetPosition(pos);
				if (len < 30)
				{
					attackData.rushFlag[0] = true;
				}
			}
			else
			{
				attackData.rushFlag[0] = true;
			}
		}
		if (attackData.rushFlag[0])
		{
			if (animNo != static_cast<int>(ANIM::FATTACK3))
			{
				animNo = static_cast<int>(ANIM::FATTACK3);
				enemyObj->SetAnim(animNo, false);
				attackData.rushFlag[1] = true;
				EffectObj::GetInstance().Stop(EffectObj::TYPE::TST);
			}
		}
		if (attackData.rushFlag[1])
		{
			enemyObj->SetAttackFlag(true);
			enemyObj->SetHighAttackFlag(true);
			if (!enemyObj->GetAnimContinue())
			{
				enemyObj->SetHighAttackFlag(false);
				enemyObj->SetAttackFlag(false);
				SetChoiceState();
			}
		}
		break;
	}
}
void Enemy::ThunderAttack(float elapsedTime)
{
	switch (actionState)
	{
	case ACTION::FIRST:
		waiteTime += elapsedTime;
		if (thunderData.exist)
		{
			for (int i = 0; i < THUNDERNUM; i++)
			{
				thunderData.pos[0] = playerPos;
				thunderData.w[i] = InQuad(waiteTime, attackData.chargeTime, 10.0f, 0.f);
			}
		}
		if (waiteTime > attackData.chargeTime)
		{
			waiteTime = 0;
			actionState = ACTION::SECOND;
			EffectObj::GetInstance().SetPosition(EffectObj::TYPE::THUNDER, DirectX::XMFLOAT3(playerPos));
			EffectObj::GetInstance().SetScale(EffectObj::TYPE::THUNDER, DirectX::XMFLOAT3(3, 10, 3));
			EffectObj::GetInstance().SetColor(EffectObj::TYPE::THUNDER, DirectX::XMFLOAT4(0.4f, 1.f, 1.f, 1.f));
			EffectObj::GetInstance().Play(EffectObj::TYPE::THUNDER);
		}
		break;
	case ACTION::SECOND:
		waiteTime += elapsedTime;
		if (waiteTime > thunderData.attackStartTime)
		{
			if (!enemyObj->GetDamageFlag())enemyObj->SetAttackFlag(true);
			for (int i = 0; i < THUNDERNUM; i++)
			{
				thunderData.w[i] = 0.f;
			}
		}
		if (waiteTime > thunderData.attackEndTime)
		{
			enemyObj->SetAttackFlag(false);
			thunderData.exist = false;
		}
		if (waiteTime > thunderData.endTime)
		{
			if (EffectObj::GetInstance().Exist(EffectObj::TYPE::THUNDER))
			{
				EffectObj::GetInstance().Stop(EffectObj::TYPE::THUNDER);
			}
			SetChoiceState();
		}
		break;
	}

}
void Enemy::ShotAttack(float elapsedTime)
{
	CalculateLength();
	CalculateDotCross();
}
void Enemy::SetAccelState()
{

}
void Enemy::UpdateAccelState(float elapsedTime)
{
}
//�m�b�N�o�b�N
void Enemy::SetKnockBackState()
{
	state = STATE::KNOCKBACK;
}

void Enemy::UpdateKnockBackState(float elapsedTime)
{
	float k = enemyObj->GetKnockBackSpeed();

	DirectX::XMFLOAT3 pos = enemyObj->GetPosition();
	DirectX::XMFLOAT3 dir = enemyObj->GetDirection();
	static float t = 0;
	t += elapsedTime;
	if (t > enemyObj->GetKnockBackTime())
	{
		SetWaitState();
		enemyObj->SetKnockBackFlag(false);
		t = 0;
	}
	pos.x = pos.x + dir.x * k;
	pos.z = pos.z + dir.z * k;
	enemyObj->SetPosition(pos);
}
void Enemy::CalculateLength()
{
	DirectX::XMFLOAT3 pos = Camera::GetInstance().GetTargetPos();
	pos.y = 0; //x,z�łƂ�ꍇy�͏����Ă���
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&enemyObj->GetPosition());
	vec = DirectX::XMVectorSubtract(p, e);
	dir = DirectX::XMVector3Normalize(vec);//�P�ʃx�N�g����
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);//���K��
	DirectX::XMStoreFloat(&len, length);
}
void Enemy::CalculateDotCross()
{
	DirectX::XMFLOAT3 pos = Camera::GetInstance().GetTargetPos();
	pos.y = 0; //x,z�łƂ�ꍇy�͏����Ă���
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&enemyObj->GetPosition());
	vec = DirectX::XMVectorSubtract(p, e);
	dir = DirectX::XMVector3Normalize(vec);//�P�ʃx�N�g����

	DirectX::XMFLOAT3 a = enemyObj->GetAngle();
	DirectX::XMFLOAT3 n = DirectX::XMFLOAT3(sinf(a.y), 0, cosf(a.y));
	DirectX::XMVECTOR vn = DirectX::XMLoadFloat3(&n);
	vn = DirectX::XMVector3Normalize(vn);//�P�ʃx�N�g����
	DirectX::XMVECTOR d = DirectX::XMVector3Dot(vn, dir);//���ςłł�̂�cos�� -180�`180
	DirectX::XMStoreFloat(&dot, d);
	if (dot > 1)dot = 1;
	if (dot < -1)dot = -1;
	DirectX::XMVECTOR c = DirectX::XMVector3Cross(vn, dir);
	DirectX::XMStoreFloat3(&cross, c);
	//	DirectX::XMFLOAT3 pos = Camera::GetInstance().GetTargetPos();
	//
	//	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&pos);
	//
	//	DirectX::XMVECTOR d, c;
	//
	//	//���ς����߂�
	//	DirectX::XMFLOAT3 front = enemyObj->GetFront();
	//	DirectX::XMVECTOR f = DirectX::XMLoadFloat3(&front);
	//
	//	d = DirectX::XMVector3Dot(p, f);
	//	//�O�ς����߂�
	//	c = DirectX::XMVector3Cross(p, f);
	//	//���߂����ς�float�^�ɂ���
	//	
	//	DirectX::XMStoreFloat(&dot, d);
	//	//���߂��O�ς�float3�^�ɂ���
	//	DirectX::XMStoreFloat3(&cross, c);
}

