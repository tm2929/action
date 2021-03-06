#include "Player.h"
#include "gamepad.h"
#include "Camera.h"
#include "KeyInput.h"
#include "Particle.h"
#include "VectorCombo.h"
#include "HitAreaRnder.h"
#include "EffectManager.h"
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
	//武器
	weapon = std::make_unique<Weapon>(weaponResource);

	playerObj->SetHitStateFlag(false);

	//data初期化
	hitData.Init();
	jumpData.Init();
	speedData.Init();
	energyData.Init();
	skillData.Init();
	//初期設定
	playerObj->SetPosition(DirectX::XMFLOAT3(3.0f, 0.f, 210.0f));
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
	//MP
	playerObj->SetMp(energyData.mp);
	playerObj->SetMaxMp(energyData.maxMp);
	//攻撃力
	power = hitData.damage;
	powerX = .0f;
	powerY = .0f;

	hitColor = DirectX::XMFLOAT4(1, 1, 1, 1);
	hitFalseColor = DirectX::XMFLOAT4(1, 1, 1, 1);//当たってない
	hitTrueColor = DirectX::XMFLOAT4(1, 0, 0, 1);//当たった
	//移動
	playerObj->SetAcceleration(speedData.acceleration);
	playerObj->SetDeceleration(speedData.deceleration);
	playerObj->SetMaxMoveSpeed(speedData.maxMoveSpeed);
	//加速
	playerObj->SetAccelTime(speedData.accelTime);
	playerObj->SetAccelSpeed(speedData.accelSpeed);
	//ノックバック
	playerObj->SetKnockBackFlag(false);
	playerObj->SetKnockBackSpeed(1.f);
	playerObj->SetKnockBackTime(0.2f);
	animNo = 0;
	playerObj->GetModel()->PlayAnimation(animNo, true);
	//エネミーのふっとび率
	playerObj->SetEnmKnockBack(0);

	state = STATE::WAIT;
	prevState = state;
	action = ACTION::FIRST;
	//当たり判定
	playerObj->SetHitAreaLeng(hitData.hitLength);
	playerObj->SetHitAttackAreaLeng(hitData.hitAttackLength);

	//playerObj->SetHitTargetAreaLeng(hitData.hitTargetLength);
	playerObj->HitAreaTransform();
	playerObj->CalculateTransform();
	playerObj->BoneNameInput();

	playerObj->SetTrajectoryFlag(false);

	animSpeed = defaultSpeed;
	for (int i = 0; i < static_cast<int>(ANIM::END); i++)
	{
		animSpeeds[i] = 1.0f;
	}
	LoadAnimSpeed();
	len = 0;
	vec = { 0,0,0 };

	weapon->CalculateTransform();
}
void Player::Update(float elapsedTime)
{
	Imgui();

	//HP,SP,MP更新処理
	HpSpMpUpdate(elapsedTime);
	//アニメーション更新
	playerObj->AnimUpdate(elapsedTime * animSpeed);
	//アニメーション時間取得
	animTime = playerObj->GetModel()->GetAnimationSeconds();
	animSpeed = animSpeeds[animNo];

	playerObj->HitAttackTransformIMGUI(false);
	if (state == STATE::KICK) playerObj->HitAttackTransformIMGUI(true);

	//playerObj->CalculateTransform();
	playerObj->HitAreaTransform();

	weapon->SetPosition(playerObj->GetHitSphere().position);
	//軌跡
	{
		Model::Node node;
		node = GetModel()->GetNodes()[trajectorynNodeNum]; //出現ノード取得
		//軌跡位置設定
		trajectoryEndPos.x = node.worldTransform._41;
		trajectoryEndPos.y = node.worldTransform._42;
		trajectoryEndPos.z = node.worldTransform._43;

		trajectoryStartPos.x = trajectoryEndPos.x + ((node.worldTransform._31) * trajectoryLen);
		trajectoryStartPos.y = trajectoryEndPos.y + ((node.worldTransform._32) * trajectoryLen);
		trajectoryStartPos.z = trajectoryEndPos.z + ((node.worldTransform._33) * trajectoryLen);
	}
	//武器更新
	weapon->Update(elapsedTime, playerObj);
	//正面取得
	playerObj->SetFront(DirectX::XMFLOAT3(sinf(angle.y), 0.f, cosf(angle.y)));
	switch (state)
	{
		//待機
	case STATE::WAIT:
		UpdateWaitState(elapsedTime);
		break;
		//ダッシュ
	case STATE::RUN:
		UpdateRunState(elapsedTime);
		break;
		//キック
	case STATE::KICK:
		UpdateKickState(elapsedTime);
		break;
		//攻撃
	case STATE::ATTACK:
		UpdateAttackState(elapsedTime);
		break;
	case STATE::JUMPATTACK:
		UpdateJumpAttackState(elapsedTime);
		break;
		//加速
	case STATE::ACCEL:
		UpdateAccelState(elapsedTime);
		break;
		//突進
	case STATE::RUSH:
		UpdateRushState(elapsedTime);
		break;
		//ヒット
	case STATE::HIT:
		UpdateHitState(elapsedTime);
		break;
	case STATE::KNOCKBACK:
		UpdateKnockBackState(elapsedTime);
		break;
		//死亡
	case STATE::DEATH:
		UpdateDeathState(elapsedTime);
		break;
		//回復スキル
	case STATE::HEAL:
		UpdateHealState(elapsedTime);
		break;
		//スキル
	case STATE::SUKILL:
		UpdateSukillState(elapsedTime);
		break;
		//tst用
	case STATE::TST:
		break;
	}
	//攻撃時以外攻撃flag常にfalse
	if (state != STATE::ATTACK && state != STATE::RUSH && state != STATE::JUMPATTACK)
	{
		playerObj->SetAttackFlag(false);
		playerObj->SetJumpAttackFlag(false);
	}
	if (state != STATE::SUKILL)
	{
		playerObj->SetSukillHitFlag(false);
	}
	//ヒット時ヒットフラグtrue
	if (state == STATE::HIT || state == STATE::KNOCKBACK)
	{
		playerObj->SetHitStateFlag(true);

	}
	else playerObj->SetHitStateFlag(false);

	//攻撃力
	power = hitData.damage +hitData.plusDamage;
	playerObj->SetPower(power);

	//当たり判定表示
	if (playerObj->GetHitFlag()) hitColor = hitTrueColor;
	else hitColor = hitFalseColor;

	if (animStopFlag) //trueならアニメーション停止
	{
		animSpeed = 0;

	}
	HitAreaRnder::GetInctance().SetHitCylinder(playerObj->GetHitArea().min, playerObj->GetHitArea().max, playerObj->GetHitArea().area, hitColor);
	HitAreaRnder::GetInctance().SetHitSphere(playerObj->GetHitSphere().position, playerObj->GetHitSphere().area, hitColor);
	//敵の攻撃がプレイヤーにあたっていたら
	if (playerObj->GetKnockBackFlag() && playerObj->GetHitFlag())
	{
		if (playerObj->GetKnockBackType() == 0) //ノックバックstate選択(相手の攻撃にあわせて二種選択)
		{
			SetHitState();
		}
		else
		{
			//SetHitState();
			SetKnockBackState();
		}
	}
	//常false 食らった瞬間だけtrueになるように
	playerObj->SetHitFlag(false);

	//落下処理
	FallOff(elapsedTime);
}

void Player::Imgui()
{
#ifdef USE_IMGUI
	ImGui::Begin("player");
	//位置
	if (ImGui::CollapsingHeader(u8"位置"))
	{
		ImGui::Text("pos.x%.f", playerObj->GetPosition().x);
		ImGui::Text("pos.y%.f", playerObj->GetPosition().y);
		ImGui::Text("pos.z%.f", playerObj->GetPosition().z);

		ImGui::Text("pos1.x%.f", pos.x);
		ImGui::Text("pos1.y%.f", pos.y);
		ImGui::Text("pos1.z%.f", pos.z);
		;
	}
	if (ImGui::CollapsingHeader("HP,SP,MP"))
	{
		ImGui::Text("hp%.f", playerObj->GetHp());
		ImGui::InputFloat("HP", &energyData.hp, 1.f);
		playerObj->SetHp(energyData.hp);
		ImGui::InputFloat("plusSp", &energyData.plusSp, 1.f);
		ImGui::InputFloat("dashSp", &energyData.dashSp, 1.f);
		ImGui::InputFloat("accelSp", &energyData.accelSp, 1.f);
		ImGui::Text("mp%.f", playerObj->GetMp());
		ImGui::InputFloat("MP", &energyData.plusMp, 1.f);
	}
	ImGui::Text("state%d", state);
	ImGui::Text("count%f", count);
	ImGui::Text("blendTime%.f", playerObj->GetModel()->GetBlendTime());
	ImGui::Text("blendFlag%d", playerObj->GetModel()->GetBlendFlag());
	ImGui::Text("blendFlag%d", playerObj->GetSukillAttackFlag());
	if (ImGui::Button(u8"sukillAttackFlag"))
	{
		if (playerObj->GetSukillAttackFlag()) playerObj->SetSukillAttackFlag(false);
		else playerObj->SetSukillAttackFlag(true);
	}
	if (ImGui::CollapsingHeader(u8"当たり安定"))
	{
		ImGui::Text("attackFlag%d", playerObj->GetAttackFlag());
		ImGui::Text("damageFlag%d", playerObj->GetDamageFlag());

		ImGui::Text("hit%d", playerObj->GetHitFlag());
		ImGui::Text("HitSpherex%f", playerObj->GetHitSphere().position.x);
		ImGui::Text("HitSpherey%f", playerObj->GetHitSphere().position.y);
		ImGui::Text("HitSpherez%f", playerObj->GetHitSphere().position.z);
		//static float foo = 1.0f;
		ImGui::Text("power%f", playerObj->GetPower());
		ImGui::InputFloat("damage", &hitData.damage, 0.1f);
		ImGui::SliderFloat(u8"当たり判定サイズ", &hitData.hitLength, 0, 50.f);
		playerObj->SetHitAreaLeng(hitData.hitLength);

		ImGui::SliderFloat(u8"攻撃当たり判定サイズ", &hitData.hitAttackLength, 0, 50.f);
		playerObj->SetHitAttackAreaLeng(hitData.hitAttackLength);
		weapon->SetHitAreaLeng(hitData.hitAttackLength);
	}
	ImGui::Text("attackFlag %d", playerObj->GetAttackFlag());
	ImGui::Text("damageFlag %d", playerObj->GetDamageFlag());
	ImGui::Text("JumpAttackFlag %d", playerObj->GetJumpAttackFlag());
	if (ImGui::CollapsingHeader(u8"速度"))
	{
		if (ImGui::CollapsingHeader(u8"通常移動"))
		{
			ImGui::SliderFloat(u8"最大速度", &speedData.maxMoveSpeed, 50.f, 100.0f);
			ImGui::SliderFloat(u8"加速度", &speedData.acceleration, 0.f, 100.f);
			ImGui::SliderFloat(u8"減速度", &speedData.deceleration, 0.f, 100.f);
		}
		if (ImGui::CollapsingHeader(u8"ダッシュ"))
		{
			ImGui::SliderFloat(u8"d最大速度", &speedData.dashMaxMoveSpeed, 50.f, 100.0f);
			ImGui::SliderFloat(u8"d加速度", &speedData.dashAcceleration, 0.f, 100.f);
			ImGui::SliderFloat(u8"d減速度", &speedData.dashDeceleration, 0.f, 100.f);
		}
		if (ImGui::CollapsingHeader(u8"ジャンプ攻撃"))
		{
			ImGui::InputFloat(u8"frontSpeed", &jumpData.frontSpeed, 0.1f);
			ImGui::InputFloat(u8"jumpAttackTime", &jumpData.jumpAttackTime, 0.1f);
			ImGui::InputFloat(u8"jumpInitialSpeed", &jumpData.jumpInitialSpeed, 0.1f);
		}
		ImGui::InputFloat(u8"waiteTime", &hitData.waiteTime, 0.1f);
		ImGui::InputFloat(u8"加速時間", &speedData.accelTime, 0.1f);
		ImGui::InputFloat(u8"加速時速度", &speedData.accelSpeed, 0.1f);
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
	animSpeed = defaultSpeed;
	state = STATE::WAIT;
	prevState = state;
	animNo = static_cast<int>(ANIM::WAIT2);
	playerObj->SetAnim(animNo, true);
}

void Player::UpdateWaitState(float elapsedTime)
{
	MoveInput();
	float power = sqrtf(powerX * powerX + powerY * powerY);
	//run
	if (power > InputSensitivity)
	{
		SetRunState();
	}
	//攻撃
	if (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X))
	{
		SetAttackState();
	}
	//回避
	if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A))
	{
		SetAccelState();
	}
	//ジャンプ攻撃
	if (KeyInput::KeyTrigger() & KEY_F || input::ButtonRisingState(0, input::PadLabel::Y))
	{
		SetJumpAttackState();
	}
#ifdef USE_IMGUI
	if (KeyInput::KeyTrigger() & KEY_E)
	{
		SetSukillState();
	}
	if (KeyInput::KeyTrigger() & KEY_Q)
	{
		SetHealState();
	}
#endif
	//MPマックス時のみ発動
	if (playerObj->GetMpMaxFlag())
	{
		//スキル
		if (KeyInput::KeyTrigger() & KEY_E || input::ButtonRisingState(0, input::PadLabel::RSHOULDER))
		{
			SetSukillState();
		}
		if (KeyInput::KeyTrigger() & KEY_Q || input::ButtonRisingState(0, input::PadLabel::LSHOULDER))
		{
			SetHealState();
		}
	}
	//視野角変更
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
	animSpeed = defaultSpeed;
	playerObj->GetModel()->SetBlendTime(BLENDNORMAL);

	playerObj->SetAcceleration(speedData.acceleration);
	playerObj->SetDeceleration(speedData.deceleration);
	playerObj->SetMaxMoveSpeed(speedData.maxMoveSpeed);

	pos = playerObj->GetPosition();

}

void Player::UpdateRunState(float elapsedTime)
{
	CalculateDotCrossLength();
	MoveInput();
	float power = sqrtf(powerX * powerX + powerY * powerY);
	if (power > InputSensitivity)
	{
		powerX /= power;
		powerY /= power;
		Move(elapsedTime, powerX, powerY);
	}
	else
	{
		SetWaitState();
	}
	if (!playerObj->GetTiredFlag())
	{
		if (KeyInput::KeyState() & KEY_LSHIFT || input::ButtonPressedState(0, input::PadLabel::LTRIGGER))
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
			if (playerObj->GetSp() <= 0) playerObj->SetTiredFlag(true);
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
		if (playerObj->GetSp() > energyData.tiredSp)
			playerObj->SetTiredFlag(false);
	}
	//攻撃
	if (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X))
	{
		if (len <= 20) SetAttackState();

		else if (len < 70) SetRushState();

		else SetAttackState();
	}
	//回避
	if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A))
	{
		SetAccelState();
	}
	//ジャンプ攻撃
	if (KeyInput::KeyTrigger() & KEY_F || input::ButtonRisingState(0, input::PadLabel::Y))
	{
		SetJumpAttackState();
	}


	//視野角変更
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
	CalculateDotCrossLength();//距離、内積、外積
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
				angle.y -= acosf(dot);//acosfでシータ（角度）がでる　
			}
			else
			{
				angle.y += acosf(dot);
			}
			//向き補完
			T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&a), DirectX::XMLoadFloat(&angle.y), 0.8f);
			DirectX::XMStoreFloat(&angle.y, T);

			playerObj->SetAngle(angle);
		}
		else if (p > 0.1f)//攻撃時少しだけ向きを変えれるように
		{
			AttackChangeAngle(elapsedTime, powerX, powerY);
		}
	}
	switch (action)
	{
	case ACTION::FIRST:
		//一撃目
		hitData.plusDamage = hitData.attackPower[0];
		hitData.stopTime[3] = hitData.stopTime[0];

		playerObj->SetEnmKnockBack(0);
		//視野角変更
		DirectX::XMVECTOR T = DirectX::XMVectorSet(0, 0, 0, 0);
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().attackFoV[0]), 0.1f);
		DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);

		//モーション終わる前に押したら二撃目へ
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			animNo = static_cast<int>(ANIM::ATTACK2);
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::SECOND;
		}
		//攻撃から戻るモーションに
		else if (!playerObj->GetAnimContinue())
		{
			animNo = static_cast<int>(ANIM::ATTACK1END);
			playerObj->SetAnim(animNo, false);
			action = ACTION::FIRSTEND;

		}
		break;
	case ACTION::FIRSTEND: //戻りモーション
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //攻撃時、戻りモーション時のみ加速(回避)できる
		{
			SetAccelState();
		}
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			animNo = static_cast<int>(ANIM::ATTACK2);
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::SECOND;
			hitData.plusDamage = hitData.attackPower[2];
		}
		else if (!playerObj->GetAnimContinue())
		{
			SetWaitState();
		}
		break;
	case ACTION::SECOND:
		//視野角変更
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().attackFoV[1]), 0.1f);
		DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);

		playerObj->SetEnmKnockBack(0);
		hitData.stopTime[3] = hitData.stopTime[1];
		//モーション終わる前に押したら３撃目へ
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			animNo = static_cast<int>(ANIM::ATTACK3);;
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::THIRD;
			hitData.plusDamage = hitData.attackPower[2];
		}
		else if (!playerObj->GetAnimContinue())
		{
			animNo = static_cast<int>(ANIM::ATTACK2END);;
			playerObj->SetAnim(animNo, false);
			action = ACTION::SECONDEND;
		}
		break;
	case ACTION::SECONDEND://戻りモーション
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //攻撃時、戻りモーション時のみ加速(回避)できる
		{
			SetAccelState();
		}
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			animNo = static_cast<int>(ANIM::ATTACK3);
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::THIRD;

			hitData.plusDamage = hitData.attackPower[2];
		}
		else if (!playerObj->GetAnimContinue())
		{
			SetWaitState();
		}
		break;
	case ACTION::THIRD:
		//視野角変更
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
	case ACTION::THIRDEND://戻りモーション
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //攻撃時、戻りモーション時のみ加速(回避)できる
		{
			SetAccelState();
		}
		if (!playerObj->GetAnimContinue())
		{
			SetWaitState();
		}
		break;
	}

	//攻撃当たり判定適用
	switch (action)
	{
	case ACTION::FIRST:
		if (playerObj->GetHitFlag())hitData.plusPos = { 0,0,0 };
		else hitData.plusPos = { 0.5,0,0.5 };

		hitData.count += elapsedTime;
		if (hitData.count > 0.1)
		{

			pos.x += sinf(angle.y) * hitData.plusPos.x;
			pos.z += cosf(angle.y) * hitData.plusPos.z;
			playerObj->SetPosition(pos);

			playerObj->SetAttackFlag(true);

		}
		break;

	case ACTION::FIRSTEND:
		hitData.plusPos = { 0,0,0 };
		hitData.count = 0;
		playerObj->SetAttackFlag(false);
		playerObj->SetDamageFlag(false);
		break;

	case ACTION::SECOND:
		hitData.count += elapsedTime;
		if (hitData.count > 0.2)
		{
			pos.x += sinf(angle.y) * hitData.plusPos.x;
			pos.z += cosf(angle.y) * hitData.plusPos.z;
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
		if (playerObj->GetHitFlag())hitData.plusPos = { 0,0,0 };
		else hitData.plusPos = { 0.5,0,0.5 };

		hitData.count += elapsedTime;
		if (hitData.count > 0.4)
		{
			playerObj->SetAttackFlag(true);

			pos.x += sinf(angle.y) * hitData.plusPos.x;
			pos.z += cosf(angle.y) * hitData.plusPos.z;
			playerObj->SetPosition(pos);
		}
		break;
	case ACTION::THIRDEND:
		hitData.count = 0;
		playerObj->SetAttackFlag(false);
		playerObj->SetDamageFlag(false);
		hitData.plusPos = { 0,0,0 };
		break;
	}
	playerObj->SetPower(power);
}
#else
void Player::SetJumpAttackState()
{
	action = ACTION::FIRST;
	state = STATE::JUMPATTACK;
	animNo = static_cast<int>(ANIM::SUKILL2);
	playerObj->SetAnim(animNo, false);
	count = 0;
	playerObj->SetJumpAttackFlag(false);
	playerObj->SetDamageFlag(false);
	hitData.plusDamage = hitData.attackPower[2];
}
void Player::UpdateJumpAttackState(float elapsedTime)
{
	count += elapsedTime;
	CalculateDotCrossLength();//距離、内積、外積
	MoveInput();
	float p = sqrtf(powerX * powerX + powerY * powerY);

	if (count < attackAngleTime)
	{
		if (p > InputSensitivity)
		{
			AttackChangeAngle(elapsedTime, powerX, powerY);
		}
		else if (len < withinDistance) //一定距離内の場合エネミーのほうを向く
		{
			DirectX::XMFLOAT3 toEnemyAngle;
			DirectX::XMStoreFloat3(&toEnemyAngle, vec);
			angle.y = atan2f(toEnemyAngle.x, toEnemyAngle.z);
			//向き補完
			playerObj->SetAngle(angle);
		}
	}
	switch (action)
	{
	case ACTION::FIRST:
		//加速
		CalculateDotCrossLength();
		pos.y += jumpData.jumpInitialSpeed + jumpData.gravity * count * count;
		if (count >= jumpData.jumpAttackTime)
		{

			if (!playerObj->GetJumpAttackFlag())//一度だけ通るように
			{
				playerObj->SetJumpAttackFlag(true);
				playerObj->SetDamageFlag(false);
			}
		}
		if (playerObj->GetHitFlag()) //ヒット時アニメーション振りまで
		{
			animTime = jumpData.jumpAttackTime;
		}
		else
		{
			if (pos.y >= 0)//前進
			{
				pos.x += sinf(angle.y) * jumpData.frontSpeed * elapsedTime;
				pos.z += cosf(angle.y) * jumpData.frontSpeed * elapsedTime;
			}
		}
		pos.y = pos.y <= 0 ? 0.0f : pos.y; //0以下なら0に

		playerObj->SetPosition(pos);
		playerObj->SetAngle(angle);
		if (!playerObj->GetAnimContinue() && pos.y <= 0)
		{
			playerObj->SetJumpAttackFlag(false);

			playerObj->GetModel()->SetBlendTime(BLENDATTACK);
			action = ACTION::FIRSTEND;
			animNo = static_cast<int>(ANIM::WAIT2);
			playerObj->SetAnim(animNo, false);
		}
		playerObj->SetPower(power);
		break;
	case ACTION::FIRSTEND:
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //攻撃時、戻りモーション時のみ加速(回避)できる
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
}
void Player::SetKickState()
{
	animNo = static_cast<int>(ANIM::KICK);
	playerObj->SetAnim(animNo, false);
	action = ACTION::FIRST;
	state = STATE::KICK;
	count = 0;
	//for (int i = 0; i < 120; i++)
	//{
	//	pParticleManager->Add_Board(std::make_shared<ParticleAccel>(), pParticleManager->Lp, playerObj->GetPosition());
	//}
	playerObj->SetAttackFlag(true);
}
void Player::UpdateKickState(float elapsedTime)
{
	count += elapsedTime;
	if (count < 0.4)
		animSpeeds[animNo] = 2.0f;
	else
		animSpeeds[animNo] = 1.6f;

	switch (action)
	{
	case Player::ACTION::FIRST:
		if (!playerObj->GetAnimContinue())
		{
			//playerObj->GetModel()->SetBlendTime(30.0f);
			animNo = static_cast<int>(ANIM::WAIT2);
			playerObj->SetAnim(animNo, false);
			action = ACTION::FIRSTEND;
			playerObj->SetAttackFlag(false);
		}
		break;
	case Player::ACTION::FIRSTEND:
		if (!playerObj->GetModel()->GetBlendFlag())
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetWaitState();
		}
		break;
	}

}
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
	playerObj->SetDamageFlag(false);
	playerObj->SetJumpAttackFlag(false);
	pos = playerObj->GetPosition();
	angle = playerObj->GetAngle();
	prevState = state;

	//SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::ATTACK_VOICE1), false);
	SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::SWING), false);
}
void Player::UpdateAttackState(float elapsedTime)
{
	CalculateDotCrossLength();//距離、内積、外積
	count += elapsedTime;
	MoveInput();
	float p = sqrtf(powerX * powerX + powerY * powerY);

	// 攻撃時少しだけ向きを変えれるように
	if (count < attackAngleTime)
	{
		if (p > InputSensitivity)
		{
			AttackChangeAngle(elapsedTime, powerX, powerY);
		}
		else if (len < withinDistance) //一定距離内の場合エネミーのほうを向く
		{
			DirectX::XMFLOAT3 toEnemyAngle;
			DirectX::XMStoreFloat3(&toEnemyAngle, vec);
			angle.y = atan2f(toEnemyAngle.x, toEnemyAngle.z);
			//向き補完
			playerObj->SetAngle(angle);
		}
	}
	switch (action)
	{
	case ACTION::FIRST:
		//一撃目
		hitData.plusDamage = hitData.attackPower[0];
		hitData.stopTime[3] = hitData.stopTime[0];

		playerObj->SetEnmKnockBack(0);
		//視野角変更
		DirectX::XMVECTOR T = DirectX::XMVectorSet(0, 0, 0, 0);
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().attackFoV[0]), 0.1f);
		DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);
		//モーション終わる前に押したら二撃目へ
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
		//攻撃から戻るモーションに
		else if (!playerObj->GetAnimContinue())
		{
			playerObj->GetModel()->SetBlendTime(BLENDATTACK);
			animNo = static_cast<int>(ANIM::WAIT2);
			playerObj->SetAnim(animNo, false);
			action = ACTION::FIRSTEND;
			//animSpeed = 1.0f;
		}
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //攻撃時、戻りモーション時のみ加速(回避)できる
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetAccelState();
		}
		break;
	case ACTION::FIRSTEND: //戻りモーション
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //攻撃時、戻りモーション時のみ加速(回避)できる
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetAccelState();
		}
		//二撃目へ
		if (playerObj->GetModel()->GetBlendFlag() && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			//animSpeed = 1.5f;
			animNo = static_cast<int>(ANIM::ATTACK2);
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::SECOND;
			hitData.plusDamage = hitData.attackPower[2];
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
		//視野角変更
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().attackFoV[1]), 0.1f);
		DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);

		playerObj->SetEnmKnockBack(0);
		hitData.stopTime[3] = hitData.stopTime[1];
		//モーション終わる前に押したら３撃目へ
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			playerObj->GetModel()->SetBlendTime(5);
			animNo = static_cast<int>(ANIM::ATTACK3);;
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::THIRD;
			hitData.plusDamage = hitData.attackPower[2];
			playerObj->SetAttackFlag(false);
			//SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::ATTACK_VOICE3), false);
			SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::SWING), false);
		}
		else if (!playerObj->GetAnimContinue())
		{
			playerObj->GetModel()->SetBlendTime(BLENDATTACK);
			animNo = static_cast<int>(ANIM::WAIT2);;
			playerObj->SetAnim(animNo, false);
			action = ACTION::SECONDEND;
			//animSpeed = 1.0f;
		}
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //攻撃時、戻りモーション時のみ加速(回避)できる
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetAccelState();
		}
		break;
	case ACTION::SECONDEND://戻りモーション
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //攻撃時、戻りモーション時のみ加速(回避)できる
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			SetAccelState();
		}
		//３撃目へ
		if (count > hitData.attackTime && (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X)))
		{
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			animNo = static_cast<int>(ANIM::ATTACK3);
			playerObj->SetAnim(animNo, false);
			count = 0;
			action = ACTION::THIRD;
			hitData.plusDamage = hitData.attackPower[2];
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
		//視野角変更
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&Camera::GetInstance().FoV), DirectX::XMLoadFloat(&Camera::GetInstance().attackFoV[2]), 0.1f);
		DirectX::XMStoreFloat(&Camera::GetInstance().FoV, T);

		hitData.stopTime[3] = hitData.stopTime[2];
		playerObj->SetEnmKnockBack(0.05f);
		if (!playerObj->GetAnimContinue())
		{
			playerObj->GetModel()->SetBlendTime(BLENDATTACK);
			animNo = static_cast<int>(ANIM::WAIT2);
			playerObj->SetAnim(animNo, false);
			action = ACTION::THIRDEND;
			//animSpeed = 1.0f;
		}

		break;
	case ACTION::THIRDEND://戻りモーション
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //攻撃時、戻りモーション時のみ加速(回避)できる
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
	//攻撃当たり判定適用
	if (state == STATE::ATTACK)
	{
		switch (action)
		{
		case ACTION::FIRST:
			if (playerObj->GetHitFlag())hitData.plusPos = { 0,0,0 };
			else hitData.plusPos = hitData.firstPlusPos;
			if (animTime < hitData.firstAnimTime)//振り始め
			{
				animSpeeds[animNo] = hitData.firstAnimSpeed[0];
			}
			else //それ以降
			{
				animSpeeds[animNo] = hitData.firstAnimSpeed[1];
			}
			if (count > hitData.firstHitStartTime)
			{
				pos.x += sinf(angle.y) * hitData.plusPos.x * elapsedTime;
				pos.z += cosf(angle.y) * hitData.plusPos.z * elapsedTime;
				playerObj->SetPosition(pos);

				if (!playerObj->GetAttackFlag())//一度だけ通るように
				{
					playerObj->SetAttackFlag(true);
					playerObj->SetDamageFlag(false);
				}
			}
			break;
		case ACTION::FIRSTEND:
			hitData.plusPos = { 0,0,0 };
			playerObj->SetAttackFlag(false);
			playerObj->SetDamageFlag(false);
			break;
		case ACTION::SECOND:
			if (count >= hitData.secondHitStartTime)
			{
				animSpeeds[animNo] = hitData.secondAnimSpeed[0];
				//pos.x += sinf(angle.y) * hitData.plusPos.x;
				//pos.z += cosf(angle.y) * hitData.plusPos.z;
				playerObj->SetPosition(pos);
				if (!playerObj->GetAttackFlag())//一度だけ通るように
				{
					playerObj->SetAttackFlag(true);
					playerObj->SetDamageFlag(false);
				}
			}
			else
			{
				animSpeeds[animNo] = hitData.secondAnimSpeed[1];
			}
			break;
		case ACTION::SECONDEND:
			playerObj->SetAttackFlag(false);
			playerObj->SetDamageFlag(false);
			break;
		case ACTION::THIRD:
			if (playerObj->GetHitFlag())hitData.plusPos = { 0,0,0 };
			else hitData.plusPos = hitData.thirdPlusPos;
			if (count > hitData.thirdHitEndTime)
			{
				playerObj->SetAttackFlag(false);
			}
			else if (count >= hitData.thirdHitStartTime)
			{
				pos.x += sinf(angle.y) * hitData.plusPos.x * elapsedTime;
				pos.z += cosf(angle.y) * hitData.plusPos.z * elapsedTime;
				playerObj->SetPosition(pos);
				if (!playerObj->GetAttackFlag())//一度だけ通るように
				{
					playerObj->SetAttackFlag(true);
					playerObj->SetDamageFlag(false);
				}
			}
			break;
		case ACTION::THIRDEND:
			playerObj->SetAttackFlag(false);
			playerObj->SetDamageFlag(false);
			hitData.plusPos = { 0,0,0 };
			playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
			break;
		}
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
		//加速
		CalculateDotCrossLength();

		if (cross.y < 0)
		{
			angle.y -= acosf(dot);//acosfでシータ（角度）がでる　
		}
		else
		{
			angle.y += acosf(dot);
		}
		//向き補完
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
	hitData.plusPos = { 100.0,0,100.0 };

	if (!playerObj->GetAttackFlag())//一度だけ通るように
	{
		playerObj->SetAttackFlag(true);
		playerObj->SetDamageFlag(false);
	}
	//SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::ATTACK_VOICE4), false);

}
void Player::UpdateRushState(float elapsedTime)
{
	count += elapsedTime;
	DirectX::XMVECTOR T = DirectX::XMVectorSet(0, 0, 0, 0);
	float a = angle.y;
	switch (action)
	{
	case ACTION::FIRST:
		//加速
		CalculateDotCrossLength();
		if (cross.y < 0)
		{
			angle.y -= acosf(dot);//acosfでシータ（角度）がでる　
		}
		else
		{
			angle.y += acosf(dot);
		}
		//向き補完
		T = DirectX::XMVectorLerp(DirectX::XMLoadFloat(&a), DirectX::XMLoadFloat(&angle.y), 0.1f);
		DirectX::XMStoreFloat(&angle.y, T);
		if (!playerObj->GetHitFlag())
		{
			if (count <= hitData.thirdHitEndTime)
			{
				if (playerObj->GetHitFlag())hitData.plusPos = { 0,0,0 };
				else hitData.plusPos = hitData.rushPlusPos;
				pos.x += sinf(angle.y) * hitData.plusPos.x * elapsedTime;
				pos.z += cosf(angle.y) * hitData.plusPos.z * elapsedTime;

			}
			else
			{
				playerObj->SetAttackFlag(false);
				if (KeyInput::KeyTrigger() & KEY_Z || input::ButtonRisingState(0, input::PadLabel::X))
				{
					playerObj->GetModel()->SetBlendTime(BLENDNORMAL);
					SetAttackState();
				}
				if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //攻撃時、戻りモーション時のみ加速(回避)できる
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
			animNo = static_cast<int>(ANIM::WAIT2);
			playerObj->SetAnim(animNo, false);
		}
		break;
	case ACTION::FIRSTEND:
		if (KeyInput::KeyTrigger() & KEY_X || input::ButtonRisingState(0, input::PadLabel::A)) //攻撃時、戻りモーション時のみ加速(回避)できる
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
	animSpeed = animSpeeds[animNo];
	for (int i = 0; i < 40; i++)
	{
		pParticleManager->Add_Board(std::make_shared<ParticleAccel>(), pParticleManager->Lp, playerObj->GetPosition());
	}

	playerObj->SetAccelTime(speedData.accelTime);
	playerObj->SetAccelSpeed(speedData.accelSpeed);

	playerObj->SetSp(playerObj->GetSp() - energyData.accelSp);

	pos = playerObj->GetPosition();
	playerObj->SetBeforePosition(pos);
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
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(d);//正規化
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
		FadeOut::GetInctence().MoveStart();
	}
}
void Player::SetHealState()
{
	EffectObj::GetInstance().SetScale(EffectObj::TYPE::HEAL, DirectX::XMFLOAT3(5, 5, 5));
	EffectObj::GetInstance().SetColor(EffectObj::TYPE::HEAL, DirectX::XMFLOAT4(1, 1, 1, 1));
	EffectObj::GetInstance().SetPosition(EffectObj::TYPE::HEAL, playerObj->GetPosition());
	EffectObj::GetInstance().Play(EffectObj::TYPE::HEAL);

	state = STATE::HEAL;
	animNo = static_cast<int>(ANIM::HEAL);
	playerObj->SetAnim(animNo, false);
	playerObj->SetMpCount(0);
	playerObj->SetMp(0);
	count = 0;
}
void Player::UpdateHealState(float elapsedTime)
{
	count += elapsedTime;
	float hp = playerObj->GetHp();
	if (hp >= playerObj->GetMaxHp())
	{
		hp = playerObj->GetMaxHp();
	}
	else
	{
		hp += energyData.plusHp * elapsedTime;
	}
	playerObj->SetHp(hp);
	if (count < skillData.healAnimTime[0]) //構えるまでの時間
	{
		animSpeed = skillData.healAnimSpeed[0];
	}
	else if (count < skillData.healAnimTime[1])//構えてから少し動いてる時間
	{
		animSpeed = skillData.healAnimSpeed[1];
	}
	else if (count < skillData.healAnimTime[2]) //完全ストップ
	{
		animSpeed = 0; //アニメーション停止
	}
	else
	{
		if (animNo == static_cast<int>(ANIM::HEAL))
		{
			playerObj->GetModel()->SetBlendTime(BLENDATTACK);
			animNo = static_cast<int>(ANIM::WAIT2);
			playerObj->SetAnim(animNo, false);
		}
		else
		{
			if (skillData.stateChangeTime < animTime)
			{
				SetWaitState();
			}
		}

	}
}
void Player::SetSukillState()
{
	state = STATE::SUKILL;
	animNo = static_cast<int>(ANIM::HEAL);
	playerObj->SetAnim(animNo, false);
	playerObj->SetSukillAttackFlag(true);
	count = 0;
	action = ACTION::FIRST;
	sukillCount = SUKILLCOUNT::ATTACK1;
	SoundManager::getinctance().Play(static_cast<int>(SoundManager::SOUNDGAME::THUNDER1), false);
	hitData.plusDamage = hitData.sukillPower;
	playerObj->SetAccelFlag(true); //enemy攻撃防止
	playerObj->SetMpCount(0);
	playerObj->SetMp(0);

	EffectObj::GetInstance().SetScale(EffectObj::TYPE::PLAYERCHARGE, DirectX::XMFLOAT3(5, 5, 5));
	EffectObj::GetInstance().SetColor(EffectObj::TYPE::PLAYERCHARGE, DirectX::XMFLOAT4(1, 1, 1, 1));
	EffectObj::GetInstance().SetPosition(EffectObj::TYPE::PLAYERCHARGE, DirectX::XMFLOAT3(playerObj->GetPosition().x, playerObj->GetPosition().y, playerObj->GetPosition().z));
	EffectObj::GetInstance().Play(EffectObj::TYPE::PLAYERCHARGE);
}
void Player::UpdateSukillState(float elapsedTime)
{
	switch (action)
	{
	case ACTION::FIRST: // 発動前構え
		count += elapsedTime;
		//剣光らす
		sukillColor.x = OutSine(count, skillData.sukillAnimTime[1], sukillMaxColor, 0);
		sukillColor.w = OutSine(count, skillData.sukillAnimTime[1], sukillMaxColor, 0);
		playerObj->SetSukillColor(sukillColor);

		if (count < skillData.sukillAnimTime[0]) //構えるまでの時間
		{
			animSpeed = skillData.sukillAnimSpeed[0];
		}
		else if (count < skillData.sukillAnimTime[1])//構えてから少し動いてる時間
		{
			animSpeed = skillData.sukillAnimSpeed[1];
		}
		else //完全ストップ
		{
			animSpeed = 0;
			if (/*!Camera::GetInstance().GetSukillEnemyFocusFlag() &&*/ !Camera::GetInstance().GetSukillEndFlag())
			{
				Camera::GetInstance().SetSukillEnemyFocusFlag(true);
				EffectObj::GetInstance().SetScale(EffectObj::TYPE::SUKILL, DirectX::XMFLOAT3(5, 5, 5));
				EffectObj::GetInstance().SetColor(EffectObj::TYPE::SUKILL, DirectX::XMFLOAT4(1, 1, 1, 1));
				EffectObj::GetInstance().SetPosition(EffectObj::TYPE::SUKILL, DirectX::XMFLOAT3(Camera::GetInstance().GetEnemyPos().x, Camera::GetInstance().GetEnemyPos().y - 20, Camera::GetInstance().GetEnemyPos().z));
				EffectObj::GetInstance().Play(EffectObj::TYPE::SUKILL);

				EffectObj::GetInstance().Stop(EffectObj::TYPE::PLAYERCHARGE);
				action = ACTION::SECOND;
				count = 0;
			}
		}
		break;

	case ACTION::SECOND:

		switch (sukillCount)
		{
		case SUKILLCOUNT::ATTACK1:
			count += elapsedTime;
			if (skillData.sukillEffectTime[0] < count)
			{
				playerObj->SetSukillHitFlag(true);
				playerObj->SetDamageFlag(false);
				count = 0;
				sukillCount = SUKILLCOUNT::ATTACK2;
			}
			break;
		case SUKILLCOUNT::ATTACK2:
			count += elapsedTime;
			if (skillData.sukillEffectTime[1] < count)
			{
				playerObj->SetSukillHitFlag(true);
				playerObj->SetDamageFlag(false);
				count = 0;
				sukillCount = SUKILLCOUNT::ATTACK3;
			}
			break;
		case SUKILLCOUNT::ATTACK3:
			count += elapsedTime;
			if (skillData.sukillEffectTime[2] < count)
			{
				playerObj->SetSukillHitFlag(true);
				playerObj->SetDamageFlag(false);
				count = 0;
				sukillCount = SUKILLCOUNT::ATTACK4;
			}
			break;
		case SUKILLCOUNT::ATTACK4:
			count += elapsedTime;
			if (skillData.sukillEffectTime[3] < count)
			{
				playerObj->SetSukillHitFlag(true);
				playerObj->SetDamageFlag(false);
				count = 0;
				sukillCount = SUKILLCOUNT::ATTACK5;
			}
			break;
		case SUKILLCOUNT::ATTACK5:
			count += elapsedTime;
			if (skillData.sukillEffectTime[4] < count)
			{
				playerObj->SetSukillHitFlag(true);
				playerObj->SetDamageFlag(false);
				count = 0;
			}
			break;

		}
	}
	if (Camera::GetInstance().GetSukillEndFlag())
	{
		playerObj->SetSukillAttackFlag(false);
		SetWaitState();
		EffectObj::GetInstance().Stop(EffectObj::TYPE::TST);

		sukillColor.x = defaultColor;
		sukillColor.w = defaultColor;
		playerObj->SetSukillColor(sukillColor);
		playerObj->SetAccelFlag(false);
	}
}

void Player::FallOff(float elapsedTime)
{
	if (pos.y > 0 && state != STATE::JUMPATTACK)//ジャンプ攻撃時以外落下
	{
		pos.y -= elapsedTime * jumpData.hitGravity;
		playerObj->SetPosition(pos);
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

	float speed = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);//ベクトルの長さ
	if (speed > playerObj->GetDeceleration())//減速率よりも高かったら減速させていくがこえれば減速させない
	{
		//移動
		vx = velocity.x / speed; //normalizeし向きだけを取り出す
		vy = velocity.y / speed;
		vz = velocity.z / speed;

		if (speed > playerObj->GetMaxMoveSpeed())//速度制限 maxを超えたらmaxの速度に。いきすぎないように
		{
			velocity.x = vx * playerObj->GetMaxMoveSpeed();
			velocity.y = vy * playerObj->GetMaxMoveSpeed();
			velocity.z = vz * playerObj->GetMaxMoveSpeed();
		}
		else
		{
			//減速
			velocity.x -= vx * playerObj->GetDeceleration();
			velocity.y -= vy * playerObj->GetDeceleration();
			velocity.z -= vz * playerObj->GetDeceleration();

		}
		pos = playerObj->GetPosition();

		pos.x += velocity.x * elapsedTime;
		pos.y += velocity.y * elapsedTime;
		pos.z += velocity.z * elapsedTime;


		playerObj->SetPosition(pos);


		// 回転
		angle = playerObj->GetAngle();
		//キャラの向きベクトル　
		float dx = sinf(angle.y);
		float dz = cosf(angle.y);

		float dot = (vx * dx) + (vz * dz);
		float rot = (1.0f - dot);
		//１フレームにどれだけ回転させるか
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
	stick_v = DirectX::XMVector3Normalize(stick_v);//正規化

	// カメラの前ベクトル取得
	DirectX::XMFLOAT3 f, r;

	f = DirectX::XMFLOAT3(v._13, 0, v._33);

	DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&f);


	//// カメラの右ベクトル取得
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

	//	// 位置の更新
	playerObj->SetAngle(angle);
}
void Player::CalculateDotCrossLength()
{
	DirectX::XMFLOAT3 enemyPos = Camera::GetInstance().GetEnemyPos();
	enemyPos.y = 0;
	DirectX::XMVECTOR epos = DirectX::XMLoadFloat3(&enemyPos);
	DirectX::XMVECTOR ppos = DirectX::XMLoadFloat3(&playerObj->GetPosition());
	vec = DirectX::XMVectorSubtract(epos, ppos);
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);//正規化
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);//単位ベクトル化
	DirectX::XMStoreFloat(&len, length);
	DirectX::XMFLOAT3 a = playerObj->GetAngle();
	DirectX::XMFLOAT3 n = DirectX::XMFLOAT3(sinf(a.y), 0, cosf(a.y));
	DirectX::XMVECTOR vn = DirectX::XMLoadFloat3(&n);
	DirectX::XMVECTOR d = DirectX::XMVector3Dot(vn, dir);//内積ででるのはcosθ -180〜180
	DirectX::XMStoreFloat(&dot, d);
	if (dot > 1)dot = 1;
	if (dot < -1)dot = -1;

	DirectX::XMVECTOR c = DirectX::XMVector3Cross(vn, dir);
	DirectX::XMStoreFloat3(&cross, c);
}
void Player::HpSpMpUpdate(float elapsedTime)
{
	//死亡処理
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
		sp += elapsedTime * energyData.plusSp;
	}
	playerObj->SetSp(sp);
	//mp
	float mp = playerObj->GetMp();
	if (mp >= playerObj->GetMaxMp())
	{
		if (!playerObj->GetMpMaxFlag())
		{
			EffectObj::GetInstance().SetScale(EffectObj::TYPE::MPMAX, DirectX::XMFLOAT3(5, 13, 7));
			EffectObj::GetInstance().SetColor(EffectObj::TYPE::MPMAX, DirectX::XMFLOAT4(1, 1, 1, 1));
			EffectObj::GetInstance().SetPosition(EffectObj::TYPE::MPMAX, DirectX::XMFLOAT3(playerObj->GetPosition().x, playerObj->GetPosition().y + 8, playerObj->GetPosition().z));
			EffectObj::GetInstance().Play(EffectObj::TYPE::MPMAX);
		}
		mp = playerObj->GetMaxMp();
		playerObj->SetMpMaxFlag(true);
		EffectObj::GetInstance().SetPosition(EffectObj::TYPE::MPMAX, DirectX::XMFLOAT3(playerObj->GetPosition().x, playerObj->GetPosition().y + 8, playerObj->GetPosition().z));
	}
	else
	{
		mp = playerObj->GetMpCount() * energyData.plusMp;
		playerObj->SetMpMaxFlag(false);
		if (EffectObj::GetInstance().Exist(EffectObj::TYPE::MPMAX)) EffectObj::GetInstance().Stop(EffectObj::TYPE::MPMAX);
	}
	playerObj->SetMp(mp);
}
void Player::LoadAnimSpeed()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/animSpeeds.bin", "rb") == 0)//ファイルがあれば
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





