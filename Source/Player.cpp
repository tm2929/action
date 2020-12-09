#include "Player.h"
#include "gamepad.h"
#include "Camera.h"
#include "KeyInput.h"
#include "Particle.h"
#include "VectorCombo.h"
#include "HitAreaRnder.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif
#include "Fadeout.h"
#include "Sound.h"
Player::Player(std::shared_ptr<ModelResource> resource, std::shared_ptr<ModelResource> weaponResource)
{
	playerObj = std::make_unique<PlayerObj>(resource);
	//����
	weapon = std::make_unique<Weapon>(weaponResource);

	playerObj->SetHitStateFlag(false);
	hitData.Init();
	speedData.Init();
	energyData.Init();
	//�����ݒ�
	playerObj->SetPosition(DirectX::XMFLOAT3(3.0f, 0.f, 100.0f));
	playerObj->SetScale(DirectX::XMFLOAT3(0.09f, 0.09f, 0.09f));
	playerObj->SetAngle(DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f));

	weapon->SetPosition(DirectX::XMFLOAT3(3.0f, 0.f, 100.0f));
	weapon->SetScale(DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));
	weapon->SetAngle(DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f));
	count = 0;
	//HP
	playerObj->SetHp(energyData.hp);
	playerObj->SetMaxHp(energyData.maxHp);
	//SP
	playerObj->SetSp(energyData.sp);
	playerObj->SetMaxSp(energyData.maxSp);

	//�U����
	power = hitData.damage;
	powerX = .0f;
	powerY = .0f;

	color[0] = DirectX::XMFLOAT4(1, 1, 1, 1);
	color[1] = DirectX::XMFLOAT4(1, 1, 1, 1);//�������ĂȂ�
	color[2] = DirectX::XMFLOAT4(1, 0, 0, 1);//��������
	//�ړ�
	playerObj->SetAcceleration(speedData.acceleration);
	playerObj->SetDeceleration(speedData.deceleration);
	playerObj->SetMaxMoveSpeed(speedData.maxMoveSpeed);
	//����
	playerObj->SetAccelTime(speedData.accelTime);
	playerObj->SetAccelSpeed(speedData.accelSpeed);
	//�m�b�N�o�b�N
	playerObj->SetKnockBackFlag(false);
	playerObj->SetKnockBackSpeed(1.f);
	playerObj->SetKnockBackTime(0.2f);
	animNo = 0;
	playerObj->GetModel()->PlayAnimation(animNo, true);
	//�G�l�~�[�̂ӂ��Ƃї�
	playerObj->SetEnmKnockBack(0);

	state = STATE::WAIT;
	prevState = state;
	action = ACTION::FIRST;
	//�����蔻��
	playerObj->SetHitAreaLeng(hitData.hitLength);
	playerObj->SetHitAttackAreaLeng(hitData.hitAttackLength);

	//playerObj->SetHitTargetAreaLeng(hitData.hitTargetLength);
	playerObj->HitAreaTransform();
	playerObj->CalculateTransform();
	playerObj->BoneNameInput();

	playerObj->SetTrajectoryFlag(false);

	animSpeed = 1.0f;
	for (int i = 0; i < static_cast<int>(ANIM::END); i++)
	{
		animSpeeds[i] = 1.0f;
	}

	LoadAnimSpeed();
	len = 0;
	vec = { 0,0,0 };
}
void Player::Update(float elapsedTime)
{
	Imgui();

	//���S����
	if (playerObj->GetHp() <= 0) 	playerObj->SetExist(false);
	if (!playerObj->GetExist() && state != STATE::DEATH)
	{
		SetDeathState();
	}
	//SP
	float sp = playerObj->GetSp();
	if (sp >= playerObj->GetMaxSp())
	{
		sp = playerObj->GetMaxSp();
	}
	else
	{
		sp += elapsedTime * energyData.pulsSp;
	}
	playerObj->SetSp(sp);

	//�A�j���[�V�����X�V
	playerObj->AnimUpdate(elapsedTime * animSpeed);
	//�A�j���[�V�������Ԏ擾
	animTime = playerObj->GetModel()->GetAnimationSeconds();
	animSpeed = animSpeeds[animNo];

	playerObj->CalculateTransform();
	playerObj->HitAttackTransformIMGUI();
	playerObj->HitAttackTransform();
	//playerObj->HitAreaUpdate();
	playerObj->HitAreaTransform();

	weapon->SetPosition(playerObj->GetHitSphere().position);
	Model::Node node = GetModel()->GetNodes()[47];
	//�O��
	trajectoryEndPos.x = node.worldTransform._41;
	trajectoryEndPos.y = node.worldTransform._42;
	trajectoryEndPos.z = node.worldTransform._43;

	//trajectoryStartPos.x = trajectoryEndPos+

	trajectoryStartPos.x = trajectoryEndPos.x + ((node.worldTransform._31) * 150);
	trajectoryStartPos.y = trajectoryEndPos.y + ((node.worldTransform._32) * 150);
	trajectoryStartPos.z = trajectoryEndPos.z + ((node.worldTransform._33) * 150);

	//weapon->SetAngle(DirectX::XMFLOAT3(angle));
	weapon->Update(elapsedTime, playerObj);

	playerObj->SetFront(DirectX::XMFLOAT3(sinf(angle.y), 0.f, cosf(angle.y)));
	switch (state)
	{
		//�ҋ@
	case STATE::WAIT:
		UpdateWaitState(elapsedTime);
		break;
		//�_�b�V��
	case STATE::RUN:
		UpdateRunState(elapsedTime);
		break;
		//�U��
	case STATE::ATTACK:
		UpdateAttackState(elapsedTime);
		break;
		//����
	case STATE::ACCEL:
		UpdateAccelState(elapsedTime);
		break;
		//�ːi
	case STATE::RUSH:
		UpdateRushState(elapsedTime);
		break;
		//�q�b�g
	case STATE::HIT:
		UpdateHitState(elapsedTime);
		break;
	case STATE::KNOCKBACK:
		UpdateKnockBackState(elapsedTime);
		break;
	case STATE::DEATH:
		UpdateDeathState(elapsedTime);
		break;
	case STATE::TST:
		break;
	}
	if (state != STATE::ATTACK && state != STATE::RUSH)
	{
		playerObj->SetAttackFlag(false);
	}
	if (state == STATE::HIT || state == STATE::KNOCKBACK)
	{
		playerObj->SetHitStateFlag(true);
	}
	else playerObj->SetHitStateFlag(false);

	power = hitData.damage + +hitData.pulsDamage;
	//power = 100;
	//�����蔻��\��
	if (playerObj->GetHitFlag())
	{
		color[0] = color[2];
	}
	else
	{
		color[0] = color[1];
	}

	if (animStopFlag)
	{
		animSpeed = 0;

	}
	pHitAreaRender.SetHitCylinder(playerObj->GetHitArea().min, playerObj->GetHitArea().max, playerObj->GetHitArea().area, color[0]);
	pHitAreaRender.SetHitSphere(playerObj->GetHitSphere().position, playerObj->GetHitSphere().area, color[0]);
	//�G�̍U�����v���C���[�ɂ������Ă�����
	if (playerObj->GetKnockBackFlag() && playerObj->GetHitFlag())
	{
		if (playerObj->GetKnockBackType() == 0)
		{
			SetHitState();
		}
		else
		{
			//SetHitState();
			SetKnockBackState();
		}
	}
	//��false �H������u�Ԃ���true�ɂȂ�悤��
	playerObj->SetHitFlag(false);
}

void Player::Imgui()
{
#ifdef USE_IMGUI
	ImGui::Begin("player");
	if (ImGui::CollapsingHeader("HP,SP"))
	{
		ImGui::Text("hp%.f", playerObj->GetHp());
		ImGui::InputFloat("HP", &energyData.hp, 1.f);
		playerObj->SetHp(energyData.hp);
		ImGui::InputFloat("pulsSp", &energyData.pulsSp, 1.f);
		ImGui::InputFloat("dashSp", &energyData.dashSp, 1.f);
		ImGui::InputFloat("accelSp", &energyData.accelSp, 1.f);
	}
	ImGui::Text("state%d", state);
	ImGui::Text("blendTime%.f", playerObj->GetModel()->GetBlendTime());
	ImGui::Text("blendFlag%d", playerObj->GetModel()->GetBlendFlag());
	if (ImGui::CollapsingHeader(u8"���������"))
	{
		ImGui::Text("attackFlag%d", playerObj->GetAttackFlag());
		ImGui::Text("damageFlag%d", playerObj->GetDamageFlag());

		ImGui::Text("hit%d", playerObj->GetHitFlag());
		ImGui::Text("HitSpherex%f", playerObj->GetHitSphere().position.x);
		ImGui::Text("HitSpherey%f", playerObj->GetHitSphere().position.y);
		ImGui::Text("HitSpherez%f", playerObj->GetHitSphere().position.z);
		//static float foo = 1.0f;
		ImGui::InputFloat("power", &hitData.damage, 0.1f);
		ImGui::SliderFloat(u8"�����蔻��T�C�Y", &hitData.hitLength, 0, 50.f);
		playerObj->SetHitAreaLeng(hitData.hitLength);

		ImGui::SliderFloat(u8"�U�������蔻��T�C�Y", &hitData.hitAttackLength, 0, 50.f);
		playerObj->SetHitAttackAreaLeng(hitData.hitAttackLength);
		weapon->SetHitAreaLeng(hitData.hitAttackLength);
	}
	ImGui::Text("attackFlag %d", playerObj->GetAttackFlag());
	ImGui::Text("damageFlag %d", playerObj->GetDamageFlag());
	if (ImGui::CollapsingHeader(u8"���x"))
	{
		if (ImGui::CollapsingHeader(u8"�ʏ�ړ�"))
		{
			ImGui::SliderFloat(u8"�ő呬�x", &speedData.maxMoveSpeed, 50.f, 100.0f);
			ImGui::SliderFloat(u8"�����x", &speedData.acceleration, 0.f, 100.f);
			ImGui::SliderFloat(u8"�����x", &speedData.deceleration, 0.f, 100.f);
		}
		if (ImGui::CollapsingHeader(u8"�_�b�V��"))
		{
			ImGui::SliderFloat(u8"d�ő呬�x", &speedData.dashMaxMoveSpeed, 50.f, 100.0f);
			ImGui::SliderFloat(u8"d�����x", &speedData.dashAcceleration, 0.f, 100.f);
			ImGui::SliderFloat(u8"d�����x", &speedData.dashDeceleration, 0.f, 100.f);
		}
		ImGui::InputFloat(u8"waiteTime", &hitData.waiteTime, 0.1f);
		ImGui::InputFloat(u8"��������", &speedData.accelTime, 0.1f);
		ImGui::InputFloat(u8"���������x", &speedData.accelSpeed, 0.1f);
		ImGui::Text("enmknockback%f", playerObj->GetEnmKnockBack());
	}
	if (ImGui::CollapsingHeader("ANIM"))
	{
		ImGui::SliderFloat("animTime", &animTime, 0, 5);
		static float blendT = 0;
		ImGui::InputFloat("blendTime", &blendT, 0.1f);
		if (ImGui::Button("setBlendTime"))
		{
			playerObj->GetModel()->SetBlendTime(blendT);
		}
		if (ImGui::SliderInt("animation", &animNo, 0, playerObj->GetModel()->GetResource()->GetAnimations().size() - 1))
		{
			playerObj->SetAnim(animNo, true);
		}
		ImGui::InputFloat(u8"animSpeed", &animSpeeds[animNo], 0.1f);
		if (ImGui::Button("save"))
		{
			SaveAnimSpeed();
		}
		if (ImGui::Button("load"))
		{
			LoadAnimSpeed();
		}

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
	}
	playerObj->AttackBoneNameEditor();
	ImGui::End();
#endif
}

void Player::SetWaitState()
{
	state = STATE::WAIT;
	prevState = state;
	animNo = static_cast<int>(ANIM::WAIT1);
	playerObj->SetAnim(animNo, true);
}

void Player::UpdateWaitState(float elapsedTime)
{
	MoveInput();
	float power = sqrtf(powerX * powerX + powerY * powerY);
	if (power > 0.1f)
	{
		SetRunState();
	}
	if (KeyInput::KeyTrigger() & KEY_V || input::ButtonRisingState(0, input::PadLabel::X))
	{
		SetRushState();
	}
	if (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X))
	{
		SetAttackState();
	}
	if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A))
	{
		SetAccelState();
	}
	//����p�ύX
	DirectX::XMVECTOR T = DirectX::XMVectorSet(0, 0, 0, 0);
	T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().waiteFoV), 0.1f);
	DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);

}

void Player::SetRunState()
{
	state = STATE::RUN;
	prevState = state;
	animNo = static_cast<int>(ANIM::RUN);
	playerObj->SetAnim(animNo, true);
	animSpeed = 1.f;
	playerObj->GetModel()->SetBlendTime(BLENDNORMAL);

	playerObj->SetAcceleration(speedData.acceleration);
	playerObj->SetDeceleration(speedData.deceleration);
	playerObj->SetMaxMoveSpeed(speedData.maxMoveSpeed);
}

void Player::UpdateRunState(float elapsedTime)
{
	CalculateDotCrossLength();
	MoveInput();
	float power = sqrtf(powerX * powerX + powerY * powerY);
	if (power > 0.1f)
	{
		powerX /= power;
		powerY /= power;
		Move(elapsedTime, powerX, powerY);
	}
	else
	{
		SetWaitState();
	}
	if (KeyInput::KeyState() & KEY_LSHIFT)
	{
		if (animNo == static_cast<int>(ANIM::RUN))
		{
			animNo = static_cast<int>(ANIM::RUN1);
			playerObj->SetAnim(animNo, true);

			playerObj->SetAcceleration(speedData.dashAcceleration);
			playerObj->SetDeceleration(speedData.dashDeceleration);
			playerObj->SetMaxMoveSpeed(speedData.dashMaxMoveSpeed);
		}
		playerObj->SetSp(playerObj->GetSp() - energyData.dashSp * elapsedTime);
	}
	else
	{
		if (animNo == static_cast<int>(ANIM::RUN1))
		{
			animNo = static_cast<int>(ANIM::RUN);
			playerObj->SetAnim(animNo, true);
			playerObj->SetAcceleration(speedData.acceleration);
			playerObj->SetDeceleration(speedData.deceleration);
			playerObj->SetMaxMoveSpeed(speedData.maxMoveSpeed);
		}

	}
	if (KeyInput::KeyTrigger() & KEY_V || input::ButtonRisingState(0, input::PadLabel::X))
	{
		SetRushState();
	}

	if (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X))
	{
		if (len <= 20) SetAttackState();

		else if (len < 70) SetRushState();

		else SetAttackState();
	}
	if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A))
	{
		SetAccelState();
	}

	//����p�ύX
	DirectX::XMVECTOR T = DirectX::XMVectorSet(0, 0, 0, 0);
	T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().runFoV), 0.1f);
	DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);
}

#if 0

void Player::SetAttackState()
{
	state = STATE::ATTACK;


	action = ACTION::FIRST;
	animNo = static_cast<int>(ANIM::ATTACK1);

	playerObj->SetAnim(animNo, false);
	count = 0;
	hitData.count = 0;

	playerObj->SetDamageFlag(false);
}

void Player::UpdateAttackState(float elapsedTime)
{
	CalculateDotCrossLength();//�����A���ρA�O��
	count += elapsedTime;
	MoveInput();
	float p = sqrtf(powerX * powerX + powerY * powerY);
	DirectX::XMVECTOR T = DirectX::XMVectorSet(0, 0, 0, 0);
	float a = angle.y;
	if (count < 0.3f)
	{
		if (len < 100)
		{
			if (cross.y < 0)
			{
				angle.y -= acosf(dot);//acosf�ŃV�[�^�i�p�x�j���ł�@
			}
			else
			{
				angle.y += acosf(dot);
			}
			//�����⊮
			T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&a), DirectX::XMLoadFloat(&angle.y), 0.8f);
			DirectX::XMStoreFloat(&angle.y, T);

			playerObj->SetAngle(angle);
		}
		else if (p > 0.1f)//�U������������������ς����悤��
		{
			AttackChangeAngle(elapsedTime, powerX, powerY);
		}
	}
	switch (action)
	{
	case ACTION::FIRST:
		//�ꌂ��
		hitData.pulsDamage = hitData.attackPower[0];
		hitData.stopTime[3] = hitData.stopTime[0];

		playerObj->SetEnmKnockBack(0);
		//����p�ύX
		DirectX::XMVECTOR T = DirectX::XMVectorSet(0, 0, 0, 0);
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().attackFoV[0]), 0.1f);
		DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);

		//���[�V�����I���O�ɉ�������񌂖ڂ�
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			animNo = static_cast<int>(ANIM::ATTACK2);
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::SECOND;
		}
		//�U������߂郂�[�V������
		else if (!playerObj->GetAnimContinue())
		{
			animNo = static_cast<int>(ANIM::ATTACK1END);
			playerObj->SetAnim(animNo, false);
			action = ACTION::FIRSTEND;

		}
		break;
	case ACTION::FIRSTEND: //�߂胂�[�V����
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //�U�����A�߂胂�[�V�������̂݉���(���)�ł���
		{
			SetAccelState();
		}
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			animNo = static_cast<int>(ANIM::ATTACK2);
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::SECOND;
			hitData.pulsDamage = hitData.attackPower[2];
		}
		else if (!playerObj->GetAnimContinue())
		{
			SetWaitState();
		}
		break;
	case ACTION::SECOND:
		//����p�ύX
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().attackFoV[1]), 0.1f);
		DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);

		playerObj->SetEnmKnockBack(0);
		hitData.stopTime[3] = hitData.stopTime[1];
		//���[�V�����I���O�ɉ�������R���ڂ�
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			animNo = static_cast<int>(ANIM::ATTACK3);;
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::THIRD;
			hitData.pulsDamage = hitData.attackPower[2];
		}
		else if (!playerObj->GetAnimContinue())
		{
			animNo = static_cast<int>(ANIM::ATTACK2END);;
			playerObj->SetAnim(animNo, false);
			action = ACTION::SECONDEND;
		}
		break;
	case ACTION::SECONDEND://�߂胂�[�V����
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //�U�����A�߂胂�[�V�������̂݉���(���)�ł���
		{
			SetAccelState();
		}
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			animNo = static_cast<int>(ANIM::ATTACK3);
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::THIRD;

			hitData.pulsDamage = hitData.attackPower[2];
		}
		else if (!playerObj->GetAnimContinue())
		{
			SetWaitState();
		}
		break;
	case ACTION::THIRD:
		//����p�ύX
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().attackFoV[2]), 0.1f);
		DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);

		hitData.stopTime[3] = hitData.stopTime[2];
		playerObj->SetEnmKnockBack(0.05f);
		if (!playerObj->GetAnimContinue())
		{
			animNo = static_cast<int>(ANIM::ATTACK3END);
			playerObj->SetAnim(animNo, false);
			action = ACTION::THIRDEND;
		}

		break;
	case ACTION::THIRDEND://�߂胂�[�V����
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //�U�����A�߂胂�[�V�������̂݉���(���)�ł���
		{
			SetAccelState();
		}
		if (!playerObj->GetAnimContinue())
		{
			SetWaitState();
		}
		break;
	}

	//�U�������蔻��K�p
	switch (action)
	{
	case ACTION::FIRST:
		if (playerObj->GetHitFlag())hitData.pulsPos = { 0,0,0 };
		else hitData.pulsPos = { 0.5,0,0.5 };

		hitData.count += elapsedTime;
		if (hitData.count > 0.1)
		{

			pos.x += sinf(angle.y) * hitData.pulsPos.x;
			pos.z += cosf(angle.y) * hitData.pulsPos.z;
			playerObj->SetPosition(pos);

			playerObj->SetAttackFlag(true);

		}
		break;

	case ACTION::FIRSTEND:
		hitData.pulsPos = { 0,0,0 };
		hitData.count = 0;
		playerObj->SetAttackFlag(false);
		playerObj->SetDamageFlag(false);
		break;

	case ACTION::SECOND:
		hitData.count += elapsedTime;
		if (hitData.count > 0.2)
		{
			pos.x += sinf(angle.y) * hitData.pulsPos.x;
			pos.z += cosf(angle.y) * hitData.pulsPos.z;
			playerObj->SetPosition(pos);

			playerObj->SetAttackFlag(true);

		}
		break;

	case ACTION::SECONDEND:
		hitData.count = 0;
		playerObj->SetAttackFlag(false);
		playerObj->SetDamageFlag(false);
		break;

	case ACTION::THIRD:
		if (playerObj->GetHitFlag())hitData.pulsPos = { 0,0,0 };
		else hitData.pulsPos = { 0.5,0,0.5 };

		hitData.count += elapsedTime;
		if (hitData.count > 0.4)
		{
			playerObj->SetAttackFlag(true);

			pos.x += sinf(angle.y) * hitData.pulsPos.x;
			pos.z += cosf(angle.y) * hitData.pulsPos.z;
			playerObj->SetPosition(pos);
		}
		break;
	case ACTION::THIRDEND:
		hitData.count = 0;
		playerObj->SetAttackFlag(false);
		playerObj->SetDamageFlag(false);
		hitData.pulsPos = { 0,0,0 };
		break;
	}
	playerObj->SetPower(power);
}
#else
void Player::SetAttackState()
{
	if (prevState == STATE::RUSH)
	{
		action = ACTION::THIRD;
		animNo = static_cast<int>(ANIM::ATTACK3);
		playerObj->SetAnim(animNo, false);
	}
	else
	{
		action = ACTION::FIRST;
		animNo = static_cast<int>(ANIM::ATTACK1);
		playerObj->SetAnim(animNo, false);
	}
	playerObj->SetAttackFlag(false);
	state = STATE::ATTACK;
	count = 0;
	hitData.count = 0;
	playerObj->SetDamageFlag(false);
	pos = playerObj->GetPosition();
	angle = playerObj->GetAngle();
	prevState = state;

	//SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::ATTACK_VOICE1), false);
	SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::SWING), false);
}
void Player::UpdateAttackState(float elapsedTime)
{
	CalculateDotCrossLength();//�����A���ρA�O��
	count += elapsedTime;
	MoveInput();
	float p = sqrtf(powerX * powerX + powerY * powerY);

	if (count < 0.3f)
	{
		if (p > 0.1f)//�U������������������ς����悤��
		{
			AttackChangeAngle(elapsedTime, powerX, powerY);
		}
		else if (len < 100)
		{
			DirectX::XMFLOAT3 toEnemyAngle;
			DirectX::XMStoreFloat3(&toEnemyAngle, vec);
			angle.y = atan2f(toEnemyAngle.x, toEnemyAngle.z);
			//�����⊮
			playerObj->SetAngle(angle);
		}
	}
	switch (action)
	{
	case ACTION::FIRST:
		//�ꌂ��
		hitData.pulsDamage = hitData.attackPower[0];
		hitData.stopTime[3] = hitData.stopTime[0];

		playerObj->SetEnmKnockBack(0);
		//����p�ύX
		DirectX::XMVECTOR T = DirectX::XMVectorSet(0, 0, 0, 0);
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().attackFoV[0]), 0.1f);
		DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);
		//���[�V�����I���O�ɉ�������񌂖ڂ�
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			animNo = static_cast<int>(ANIM::ATTACK2);
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::SECOND;
			playerObj->SetAttackFlag(false);
			//SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::ATTACK_VOICE2), false);
			SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::SWING), false);
		}
		//�U������߂郂�[�V������
		else if (!playerObj->GetAnimContinue())
		{
			playerObj->GetModel()->SetBlendTime(10.0f);
			animNo = static_cast<int>(ANIM::WAIT1);
			playerObj->SetAnim(animNo, false);
			action = ACTION::FIRSTEND;
			//animSpeed = 1.0f;
		}
		break;
	case ACTION::FIRSTEND: //�߂胂�[�V����
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //�U�����A�߂胂�[�V�������̂݉���(���)�ł���
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetAccelState();
		}
		if (playerObj->GetModel()->GetBlendFlag() && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			//animSpeed = 1.5f;
			animNo = static_cast<int>(ANIM::ATTACK2);
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::SECOND;
			hitData.pulsDamage = hitData.attackPower[2];
			playerObj->SetAttackFlag(false);
			//SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::ATTACK_VOICE2), false);
			SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::SWING), false);
		}
		else if (!playerObj->GetModel()->GetBlendFlag())
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetWaitState();
		}
		break;
	case ACTION::SECOND:
		//����p�ύX
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().attackFoV[1]), 0.1f);
		DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);

		playerObj->SetEnmKnockBack(0);
		hitData.stopTime[3] = hitData.stopTime[1];
		//���[�V�����I���O�ɉ�������R���ڂ�
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			playerObj->GetModel()->SetBlendTime(5);
			animNo = static_cast<int>(ANIM::ATTACK3);;
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::THIRD;
			hitData.pulsDamage = hitData.attackPower[2];
			playerObj->SetAttackFlag(false);
			//SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::ATTACK_VOICE3), false);
			SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::SWING), false);
		}
		else if (!playerObj->GetAnimContinue())
		{
			playerObj->GetModel()->SetBlendTime(BLENDATTACK);
			animNo = static_cast<int>(ANIM::WAIT1);;
			playerObj->SetAnim(animNo, false);
			action = ACTION::SECONDEND;
			//animSpeed = 1.0f;
		}
		break;
	case ACTION::SECONDEND://�߂胂�[�V����
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //�U�����A�߂胂�[�V�������̂݉���(���)�ł���
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetAccelState();
		}
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			animNo = static_cast<int>(ANIM::ATTACK3);
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::THIRD;
			hitData.pulsDamage = hitData.attackPower[2];
			playerObj->SetAttackFlag(false);
			//SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::ATTACK_VOICE3), false);
		}
		else if (!playerObj->GetModel()->GetBlendFlag())
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetWaitState();
		}
		break;
	case ACTION::THIRD:
		//����p�ύX
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().attackFoV[2]), 0.1f);
		DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);

		hitData.stopTime[3] = hitData.stopTime[2];
		playerObj->SetEnmKnockBack(0.05f);
		if (!playerObj->GetAnimContinue())
		{
			playerObj->GetModel()->SetBlendTime(BLENDATTACK);
			animNo = static_cast<int>(ANIM::WAIT1);
			playerObj->SetAnim(animNo, false);
			action = ACTION::THIRDEND;
			//animSpeed = 1.0f;
		}
		break;
	case ACTION::THIRDEND://�߂胂�[�V����
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //�U�����A�߂胂�[�V�������̂݉���(���)�ł���
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetAccelState();
		}
		if (!playerObj->GetModel()->GetBlendFlag())
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetWaitState();
		}
		break;
	}
	//�U�������蔻��K�p
	switch (action)
	{
	case ACTION::FIRST:
		if (playerObj->GetHitFlag())hitData.pulsPos = { 0,0,0 };
		else hitData.pulsPos = { 30.f,0.f,30.f };
		if (animTime < 0.4f)
		{
			animSpeeds[animNo] = 2.0f;
		}
		else
		{
			animSpeeds[animNo] = 1.5f;
		}
		if (count > 0.1)
		{
			pos.x += sinf(angle.y) * hitData.pulsPos.x * elapsedTime;
			pos.z += cosf(angle.y) * hitData.pulsPos.z * elapsedTime;
			playerObj->SetPosition(pos);

			if (!playerObj->GetAttackFlag())//��x�����ʂ�悤��
			{
				playerObj->SetAttackFlag(true);
				playerObj->SetDamageFlag(false);
			}
		}
		break;
	case ACTION::FIRSTEND:
		hitData.pulsPos = { 0,0,0 };
		playerObj->SetAttackFlag(false);
		playerObj->SetDamageFlag(false);
		break;
	case ACTION::SECOND:
		if (count >= 0.4)
		{
			animSpeeds[animNo] = 0.8f;
			//pos.x += sinf(angle.y) * hitData.pulsPos.x;
			//pos.z += cosf(angle.y) * hitData.pulsPos.z;
			playerObj->SetPosition(pos);
			if (!playerObj->GetAttackFlag())//��x�����ʂ�悤��
			{
				playerObj->SetAttackFlag(true);
				playerObj->SetDamageFlag(false);
			}
		}
		else
		{
			animSpeeds[animNo] = 1.0f;
		}
		break;
	case ACTION::SECONDEND:
		playerObj->SetAttackFlag(false);
		playerObj->SetDamageFlag(false);
		break;
	case ACTION::THIRD:
		if (playerObj->GetHitFlag())hitData.pulsPos = { 0,0,0 };
		else hitData.pulsPos = { 20.f,0.f,20.f };
		if (count > 0.8)
		{
			playerObj->SetAttackFlag(false);
		}
		else if (count >= 0.4)
		{
			pos.x += sinf(angle.y) * hitData.pulsPos.x * elapsedTime;
			pos.z += cosf(angle.y) * hitData.pulsPos.z * elapsedTime;
			playerObj->SetPosition(pos);
			if (!playerObj->GetAttackFlag())//��x�����ʂ�悤��
			{
				playerObj->SetAttackFlag(true);
				playerObj->SetDamageFlag(false);
			}
		}
		break;
	case ACTION::THIRDEND:
		hitData.count = 0;
		playerObj->SetAttackFlag(false);
		playerObj->SetDamageFlag(false);
		hitData.pulsPos = { 0,0,0 };
		playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
		break;
	}
	playerObj->SetPower(power);
}
#endif 

#if 0
void Player::SetRushState()
{
	state = STATE::RUSH;
	count = 0;
	animNo = static_cast<int>(ANIM::ACCELATTACK1);
	playerObj->SetAnim(animNo, false);
	action = ACTION::FIRST;
	playerObj->SetDamageFlag(false);
	for (int i = 0; i < 40; i++)
	{
		pParticleManager->Add_Board(std::make_shared<ParticleAccel>(), pParticleManager->Lp, playerObj->GetPosition());
	}
}

void Player::UpdateRushState(float elapsedTime)
{
	DirectX::XMVECTOR T = DirectX::XMVectorSet(0, 0, 0, 0);
	float a = angle.y;
	switch (action)
	{
	case ACTION::FIRST:
		//����
		CalculateDotCrossLength();

		if (cross.y < 0)
		{
			angle.y -= acosf(dot);//acosf�ŃV�[�^�i�p�x�j���ł�@
		}
		else
		{
			angle.y += acosf(dot);
		}
		//�����⊮
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&a), DirectX::XMLoadFloat(&angle.y), 0.8f);
		DirectX::XMStoreFloat(&angle.y, T);
		if (!playerObj->GetHitFlag())
		{
			pos.x += sinf(angle.y) * speedData.accelSpeed * elapsedTime;
			pos.z += cosf(angle.y) * speedData.accelSpeed * elapsedTime;
		}
		playerObj->SetPosition(pos);
		playerObj->SetAngle(angle);

		animSpeed = 4.f;
		count += elapsedTime;
		playerObj->SetAttackFlag(true);
		if (count > speedData.accelTime || len < 10)
		{
			count = 0;
			animSpeed = 1.f;
			action = ACTION::SECOND;
			animNo = static_cast<int>(ANIM::ACCELATTACK2);
			playerObj->SetAnim(animNo, false);
		}
		break;
	case ACTION::SECOND:
		count += elapsedTime;


		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			SetAttackState();
		}
		if (!playerObj->GetAnimContinue() && count > hitData.waiteTime)
		{
			playerObj->SetAttackFlag(false);
			SetWaitState();
		}
		break;
	}
}
#else
void Player::SetRushState()
{
	state = STATE::RUSH;
	prevState = state;
	action = ACTION::FIRST;
	animNo = static_cast<int>(ANIM::ATTACK4);
	playerObj->SetAnim(animNo, false);
	count = 0;
	hitData.count = 0;
	hitData.pulsPos = { 100.0,0,100.0 };

	if (!playerObj->GetAttackFlag())//��x�����ʂ�悤��
	{
		playerObj->SetAttackFlag(true);
		playerObj->SetDamageFlag(false);
	}
	//SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::ATTACK_VOICE4), false);

}
void Player::UpdateRushState(float elapsedTime)
{
	hitData.count += elapsedTime;
	DirectX::XMVECTOR T = DirectX::XMVectorSet(0, 0, 0, 0);
	float a = angle.y;
	switch (action)
	{
	case ACTION::FIRST:
		//����
		CalculateDotCrossLength();
		if (cross.y < 0)
		{
			angle.y -= acosf(dot);//acosf�ŃV�[�^�i�p�x�j���ł�@
		}
		else
		{
			angle.y += acosf(dot);
		}
		//�����⊮
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&a), DirectX::XMLoadFloat(&angle.y), 0.1f);
		DirectX::XMStoreFloat(&angle.y, T);
		if (!playerObj->GetHitFlag())
		{
			if (hitData.count <= 0.8)
			{
				if (playerObj->GetHitFlag())hitData.pulsPos = { 0,0,0 };
				else hitData.pulsPos = { 100.0,0,100.0 };

				pos.x += sinf(angle.y) * hitData.pulsPos.x * elapsedTime;
				pos.z += cosf(angle.y) * hitData.pulsPos.z * elapsedTime;
			}
			else
			{
				playerObj->SetAttackFlag(false);
				if (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X))
				{
					playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
					SetAttackState();
				}
				if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //�U�����A�߂胂�[�V�������̂݉���(���)�ł���
				{
					playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
					SetAccelState();
				}
			}
		}
		playerObj->SetPosition(pos);
		playerObj->SetAngle(angle);
		if (!playerObj->GetAnimContinue())
		{
			playerObj->GetModel()->SetBlendTime(BLENDATTACK);
			action = ACTION::FIRSTEND;
			animNo = static_cast<int>(ANIM::WAIT1);
			playerObj->SetAnim(animNo, false);
		}
		break;
	case ACTION::FIRSTEND:
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //�U�����A�߂胂�[�V�������̂݉���(���)�ł���
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetAccelState();
		}
		if (playerObj->GetModel()->GetBlendFlag() && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetAttackState();
		}
		else if (!playerObj->GetModel()->GetBlendFlag())
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetWaitState();
		}
		break;
	}
	playerObj->SetPower(power);
}
#endif 

#if 0
void Player::SetAccelState()
{
	state = STATE::ACCEL;
	count = 0;
	animNo = static_cast<int>(ANIM::ACCEL);
	playerObj->SetAnim(animNo, false);
	for (int i = 0; i < 40; i++)
	{
		pParticleManager->Add_Board(std::make_shared<ParticleAccel>(), pParticleManager->Lp, playerObj->GetPosition());
	}

	playerObj->SetAccelTime(0.1f);
	playerObj->SetAccelSpeed(400.f);

}
void Player::UpdateAccelState(float elapsedTime)
{
	if (playerObj->GetAccelTime() > count)
	{
		pos.x += sinf(angle.y) * playerObj->GetAccelSpeed() * elapsedTime;
		pos.z += cosf(angle.y) * playerObj->GetAccelSpeed() * elapsedTime;

		playerObj->SetPosition(pos);
		playerObj->SetAngle(angle);
		playerObj->SetAccelFlag(true);

		animSpeed = 4.f;
	}
	else
	{

		SetWaitState();
		count = 0;
		animSpeed = 1.f;
		playerObj->SetAccelFlag(false);
	}
	count += elapsedTime;
}

#else
void Player::SetAccelState()
{
	if (playerObj->GetSp() <= energyData.accelSp) return;
	state = STATE::ACCEL;
	count = 0;
	animNo = static_cast<int>(ANIM::ACCEL);
	playerObj->SetAnim(animNo, false);
	for (int i = 0; i < 40; i++)
	{
		pParticleManager->Add_Board(std::make_shared<ParticleAccel>(), pParticleManager->Lp, playerObj->GetPosition());
	}

	playerObj->SetAccelTime(2.f);
	playerObj->SetAccelSpeed(speedData.accelSpeed);

	playerObj->SetSp(playerObj->GetSp() - energyData.accelSp);
}
void Player::UpdateAccelState(float elapsedTime)
{
	if (playerObj->GetAnimContinue())
	{
		pos.x += sinf(angle.y) * playerObj->GetAccelSpeed() * elapsedTime;
		pos.z += cosf(angle.y) * playerObj->GetAccelSpeed() * elapsedTime;
		playerObj->SetPosition(pos);
		playerObj->SetAngle(angle);
		playerObj->SetAccelFlag(true);
		animSpeed = animSpeeds[animNo];
	}
	else
	{
		SetWaitState();
		animSpeed = 1.f;
		playerObj->SetAccelFlag(false);
	}
}
#endif
void Player::SetHitState()
{
	state = STATE::HIT;
	animNo = static_cast<int>(ANIM::HIT1);
	playerObj->SetAnim(animNo, false);
	count = 0;

	speedData.knockBackSpeed = 100.0f;
	playerObj->SetKnockBackSpeed(speedData.knockBackSpeed);
	playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
}
void Player::UpdateHitState(float elapsedTime)
{
	float speed = playerObj->GetKnockBackSpeed();

	pos = playerObj->GetPosition();

	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&Camera::GetInstance().GetEnemyPos());

	DirectX::XMVECTOR d = DirectX::XMVectorSubtract(p, e);
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(d);//���K��
	DirectX::XMFLOAT3 direction;
	DirectX::XMStoreFloat3(&direction, dir);
	if (count > playerObj->GetKnockBackTime())
	{
		SetWaitState();
		playerObj->SetKnockBackFlag(false);
	}

	pos.x = pos.x + direction.x * speed * elapsedTime;
	pos.z = pos.z + direction.z * speed * elapsedTime;

	playerObj->SetPosition(pos);

	count += elapsedTime;
}
void Player::SetKnockBackState()
{
	state = STATE::KNOCKBACK;
	animNo = static_cast<int>(ANIM::KNOCKBACK);
	playerObj->SetAnim(animNo, false);
	count = 0;
	pos = playerObj->GetPosition();
	speedData.knockBackSpeed = 100.0f;
	playerObj->SetKnockBackSpeed(speedData.knockBackSpeed);
	playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
}
void Player::UpdateKnockBackState(float elapsedTime)
{
	float speed = playerObj->GetKnockBackSpeed();
	if (playerObj->GetAnimContinue())
	{
		if (animTime < 1.1)
		{
			pos.x -= sinf(angle.y) * speed * elapsedTime;
			pos.z -= cosf(angle.y) * speed * elapsedTime;

			playerObj->SetPosition(pos);
		}
		animSpeed = animSpeeds[animNo];
	}
	else
	{
		SetWaitState();
		playerObj->SetKnockBackFlag(false);
		animSpeed = 1.f;
	}
}
void Player::SetDeathState()
{
	state = STATE::DEATH;
	animNo = static_cast<int>(ANIM::DIE);
	playerObj->SetAnim(animNo, false);

	animSpeed = 0.8f;
}
void Player::UpdateDeathState(float elapsedTime)
{
	if (!playerObj->GetAnimContinue())
	{
		pFadeOut.MoveStart();
	}
}
void Player::MoveInput()
{
	powerX = .0f;
	powerY = .0f;

	powerX = input::StickStateX(0, false);
	powerY = input::StickStateY(0, false);

	if (KeyInput::KeyState() & KEY_A)powerX = -1.0f;
	if (KeyInput::KeyState() & KEY_D)powerX = 1.0f;
	if (KeyInput::KeyState() & KEY_W)powerY = -1.0f;
	if (KeyInput::KeyState() & KEY_S)powerY = 1.0f;

}
void Player::Move(float elapsedTime, float pX, float pY)
{

	DirectX::XMFLOAT4X4 v = Camera::GetInstance().GetView();

	DirectX::XMFLOAT3 right = Camera::GetInstance().GetRight();
	DirectX::XMFLOAT3 front = Camera::GetInstance().GetFront();


	DirectX::XMFLOAT3 force;
	force.x = (right.x * pX) + (front.x * pY);
	force.y = 0.0f;
	force.z = (-right.z * pX) + (-front.z * pY);

	velocity.x += force.x * playerObj->GetAcceleration();
	velocity.y += force.y * playerObj->GetAcceleration();
	velocity.z += force.z * playerObj->GetAcceleration();

	float speed = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);//�x�N�g���̒���
	if (speed > playerObj->GetDeceleration())//�������������������猸�������Ă�����������Ό��������Ȃ�
	{
		//�ړ�
		vx = velocity.x / speed; //normalize���������������o��
		vy = velocity.y / speed;
		vz = velocity.z / speed;

		if (speed > playerObj->GetMaxMoveSpeed())//���x���� max�𒴂�����max�̑��x�ɁB���������Ȃ��悤��
		{
			velocity.x = vx * playerObj->GetMaxMoveSpeed();
			velocity.y = vy * playerObj->GetMaxMoveSpeed();
			velocity.z = vz * playerObj->GetMaxMoveSpeed();
		}
		else
		{
			//����
			velocity.x -= vx * playerObj->GetDeceleration();
			velocity.y -= vy * playerObj->GetDeceleration();
			velocity.z -= vz * playerObj->GetDeceleration();

		}
		pos = playerObj->GetPosition();

		pos.x += velocity.x * elapsedTime;
		pos.y += velocity.y * elapsedTime;
		pos.z += velocity.z * elapsedTime;


		playerObj->SetPosition(pos);


		// ��]
		angle = playerObj->GetAngle();
		//�L�����̌����x�N�g���@
		float dx = sinf(angle.y);
		float dz = cosf(angle.y);

		float dot = (vx * dx) + (vz * dz);
		float rot = (1.0f - dot);
		//�P�t���[���ɂǂꂾ����]�����邩
		float limit = playerObj->GetTurnSpeed();
		if (rot > limit)
		{
			rot = limit;
		}
		float cross = (vx * dz) - (vz * dx);
		if (cross > 0.0f)
		{
			angle.y += rot;
		}
		else
		{
			angle.y -= rot;
		}
		playerObj->SetAngle(angle);
	}
	else
	{
		velocity.x = 0;
		velocity.y = 0;
		velocity.z = 0;
	}
}
void Player::AttackChangeAngle(float elapsedTime, float pX, float pY)
{
	DirectX::XMFLOAT4X4 v = Camera::GetInstance().GetView();
	DirectX::XMMATRIX view;
	view = DirectX::XMLoadFloat4x4(&v);

	DirectX::XMFLOAT4 stick = { -pX,0.0f,-pY,0.0f };
	DirectX::XMVECTOR stick_v = DirectX::XMLoadFloat4(&stick);
	stick_v = DirectX::XMVector3Normalize(stick_v);//���K��

	// �J�����̑O�x�N�g���擾
	DirectX::XMFLOAT3 f, r;

	f = DirectX::XMFLOAT3(v._13, 0, v._33);

	DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&f);


	//// �J�����̉E�x�N�g���擾
	r = DirectX::XMFLOAT3(v._11, 0, v._31);

	DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&r);

	DirectX::XMVECTOR f_dot = DirectX::XMVector3Dot(forward, stick_v);
	float fangle;
	DirectX::XMStoreFloat(&fangle, f_dot);
	float  fa = acosf(fangle) / 0.01745f;

	DirectX::XMVECTOR r_dot = DirectX::XMVector3Dot(right, stick_v);
	float rangle;
	DirectX::XMStoreFloat(&rangle, r_dot);

	if (rangle > 0.0f)
	{
		angle.y = -fa * 0.01745f;
	}
	else if (rangle < 0.0f)
	{
		angle.y = fa * 0.01745f;
	}
	else
	{
		if (fangle > 0.0f)
		{
			angle.y = 0;
		}
		else
		{
			angle.y = 180.0f * 0.01745f;
		}
	}

	//	// �ʒu�̍X�V
	playerObj->SetAngle(angle);
}
void Player::CalculateDotCrossLength()
{
	DirectX::XMFLOAT3 enemyPos = Camera::GetInstance().GetEnemyPos();
	enemyPos.y = 0;
	DirectX::XMVECTOR epos = DirectX::XMLoadFloat3(&enemyPos);
	DirectX::XMVECTOR ppos = DirectX::XMLoadFloat3(&playerObj->GetPosition());
	vec = DirectX::XMVectorSubtract(epos, ppos);
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);//���K��
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);//�P�ʃx�N�g����
	DirectX::XMStoreFloat(&len, length);
	DirectX::XMFLOAT3 a = playerObj->GetAngle();
	DirectX::XMFLOAT3 n = DirectX::XMFLOAT3(sinf(a.y), 0, cosf(a.y));
	DirectX::XMVECTOR vn = DirectX::XMLoadFloat3(&n);
	DirectX::XMVECTOR d = DirectX::XMVector3Dot(vn, dir);//���ςłł�̂�cos�� -180�`180
	DirectX::XMStoreFloat(&dot, d);
	if (dot > 1)dot = 1;
	if (dot < -1)dot = -1;

	DirectX::XMVECTOR c = DirectX::XMVector3Cross(vn, dir);
	DirectX::XMStoreFloat3(&cross, c);
}
void Player::LoadAnimSpeed()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/animSpeeds.bin", "rb") == 0)//�t�@�C���������
	{
		int size = 0;
		fread(&animSpeeds, sizeof(animSpeeds), 1, fp);
		fclose(fp);
	}
}
void Player::SaveAnimSpeed()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/animSpeeds.bin", "wb");
	fwrite(&animSpeeds, sizeof(animSpeeds), 1, fp);
	fclose(fp);
}





