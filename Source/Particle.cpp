#include "Particle.h"
#include "BlendState.h"
#include "Camera.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif
ParticleManager* ParticleManager::instance = nullptr;
void ParticleObj::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir)
{
	DirectX::XMMATRIX W;
	{

		DirectX::XMMATRIX s, r, t;
		s = DirectX::XMMatrixScaling(scale, scale, scale);//スケール行列
		r = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);//回転行列
		t = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);//移動行列

		W = s * r * t;
	}

	DirectX::XMFLOAT4X4 v, p, wvp, w;
	DirectX::XMStoreFloat4x4(&v, view);
	DirectX::XMStoreFloat4x4(&p, projection);
	DirectX::XMStoreFloat4x4(&wvp, W * view * projection);
	DirectX::XMStoreFloat4x4(&w, W);

	obj->render(immediate_context, color, light_dir, wvp, w);

}
void ParticleBoard::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir)
{

	DirectX::XMFLOAT4X4 v, p;
	DirectX::XMStoreFloat4x4(&v, view);
	DirectX::XMStoreFloat4x4(&p, projection);


	board->render(immediate_context, pos, scale, angle, v, p, color);

}
void ParticleManager::Init(ID3D11Device* device)
{
	particleObjList.clear();
	particleBoardList.clear();

	obj.push_back(std::make_shared<GeometricCube>(device));
	obj.push_back(std::make_shared<GeometricSphere>(device, 32, 16));
	board.push_back(std::make_shared <Board>(device, L"Data/images/siro.png"));
	board.push_back(std::make_shared <Board>(device, L"Data/images/koa.png"));
	board.push_back(std::make_shared <Board>(device, L"Data/images/image14.png"));
	board.push_back(std::make_shared <Board>(device, L"Data/images/lp.png"));
	board.push_back(std::make_shared <Board>(device, L"Data/Images/particle.png"));
	board.push_back(std::make_shared <Board>(device, L"Data/Images/particle01.png"));
	board.push_back(std::make_shared <Board>(device, L"Data/Images/fire.png"));
	board.push_back(std::make_shared <Board>(device, L"Data/Images/fire2.png"));
	board.push_back(std::make_shared <Board>(device, L"Data/Images/koko.png"));
	//Load();

}

void ParticleManager::Update(float elapsedTime)
{
#ifdef USE_IMGUI
	ImGui::Begin("Particle");

	if (ImGui::Button("Set"))
	{
		for (int i = 0; i < 80; i++)
		{
			ParticleManager::getInstance()->Add_Board(std::make_shared<ParticleCharge>(), ParticleManager::getInstance()->Lp, DirectX::XMFLOAT3(Camera::GetInstance().GetEnemyPos().x, Camera::GetInstance().GetEnemyPos().y + 10, Camera::GetInstance().GetEnemyPos().z));
		}

	}

	ImGui::InputInt("texnum", &pParticleManager->Data.texnum, 0, 10);
	ImGui::SliderFloat("angle.x", &pParticleManager->Data.angle.x, 0, 360);
	ImGui::SliderFloat("angle.y", &pParticleManager->Data.angle.y, 0, 360);
	ImGui::SliderFloat("angle.z", &pParticleManager->Data.angle.z, 0, 360);
	ImGui::InputFloat("color.x", &pParticleManager->Data.color.x, 0.1f);
	ImGui::InputFloat("color.y", &pParticleManager->Data.color.y, 0.1f);
	ImGui::InputFloat("color.z", &pParticleManager->Data.color.z, 0.1f);
	ImGui::InputFloat("color.w", &pParticleManager->Data.color.w, 0.1f);
	ImGui::SliderFloat("scale", &pParticleManager->Data.scale, 0, 100);
	ImGui::SliderFloat("plusX", &pParticleManager->Data.plusPos.x, -50, 50);
	ImGui::SliderFloat("plusY", &pParticleManager->Data.plusPos.y, -50, 50);
	ImGui::SliderFloat("plusZ", &pParticleManager->Data.plusPos.z, -50, 50);
	ImGui::SliderInt("dispMaxTimer", &pParticleManager->Data.dispMaxTimer, 0, 50);

	static int n = 0;


	ImGui::End();

#endif
	for (auto& it : particleObjList)
	{
		it->Update(elapsedTime);
	}
	for (auto& it : particleBoardList)
	{
		it->Update(elapsedTime);
	}


	auto eraser_obj = [](const std::shared_ptr<ParticleObj>& p)
	{
		if (!p->exist) return true;
		return false;
	};
	particleObjList.erase(std::remove_if(particleObjList.begin(), particleObjList.end(), eraser_obj), particleObjList.end());

	auto eraser_board = [](const std::shared_ptr<ParticleBoard>& p)
	{
		if (!p->exist) return true;
		return false;
	};
	particleBoardList.erase(std::remove_if(particleBoardList.begin(), particleBoardList.end(), eraser_board), particleBoardList.end());
}
void ParticleManager::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/Particle2.bin", "rb") == 0)//ファイルがあれば
	{
		int size = 0;
		fread(&Data, sizeof(ParticleData), 1, fp);
		fclose(fp);
	}
}
void ParticleManager::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir)
{

	for (auto& it : particleObjList)
	{
		it->Render(immediate_context, view, projection, light_dir);
	}

	for (auto& it : particleBoardList)
	{

		it->Render(immediate_context, view, projection, light_dir);
	}
}

bool ParticleManager::Add_Obj(std::shared_ptr<ParticleObj> p, geo i, const DirectX::XMFLOAT3& pos)
{
	p->Init(pos, obj[static_cast<int>(i)]);

	particleObjList.emplace_back(p);
	return true;
}
bool ParticleManager::Add_Board(std::shared_ptr<ParticleBoard> p, tex i, const DirectX::XMFLOAT3& pos)
{
	p->Init(pos, board[static_cast<int>(i)]);

	//調整用
	//p->Init(pos, board[pParticleManager->Data.texnum]);

	particleBoardList.emplace_back(p);
	return true;
}


void ParticleAccel::Init(const DirectX::XMFLOAT3& pos, std::shared_ptr<Board> board)
{
	exist = true;
	this->board = board;

	w = 10.0f;
	color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, w);
	scale = 1.5f;

	//blender::Set(blender::NONE);
	int a = rand() % 2;
	if (a == 0)
	{
		this->pos.x = pos.x + 1;
	}
	else
	{
		this->pos.x = pos.x - 1;
	}
	int b = rand() % 2;
	if (b == 0)
	{
		this->pos.z = pos.z + 1;
	}
	else
	{
		this->pos.z = pos.z - 1;
	}

	this->pos.y = pos.y;


	speed.x = ((rand() % 2001) - 1000) * 0.001f * 80.45f;
	speed.z = ((rand() % 2001) - 1000) * 0.001f * 80.00f;
	speed.y = ((rand() % 2001) - 1000) * 0.001f * 40.00f;



	accel.x = ((rand() % 2001) - 1000) * 0.001f * 5.4f;
	accel.y = ((rand() % 2001) - 1000) * 0.001f * 5.4f;
	accel.z = ((rand() % 2001) - 1000) * 0.001f * 5.1f;



	dispTimer = 0;
	dispMaxTimer = 28;

}
void ParticleAccel::Update(float elapsedTime)
{
	int a = rand() % 2;
	if (a == 0)
	{
		speed.x += accel.x;
		speed.y -= accel.y;
		speed.z += accel.z;
	}
	else
	{
		speed.x -= accel.x;
		speed.y += accel.y;
		speed.z -= accel.z;
	}
	pos.x += speed.x * elapsedTime;
	pos.y += speed.y * elapsedTime;
	pos.z += speed.z * elapsedTime;

	scale = InSine(static_cast<float>(dispTimer), static_cast<float>(dispMaxTimer), 0.f, 1.5f);
	w = OutQuad(static_cast<float>(dispTimer), static_cast<float>(dispMaxTimer), 0.f, 10.f);
	color.w = w;



	dispTimer++;

	if (dispTimer >= dispMaxTimer)	exist = false;

}

void ParticleHit::Init(const DirectX::XMFLOAT3& pos, std::shared_ptr<Board> board)
{
	exist = true;
	this->board = board;

	w = 1.0f;
	color = DirectX::XMFLOAT4(1, 1, 1, 100);
	scale = 1.f;

	//blender::Set(blender::NONE);

	angle = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	int a = rand() % 2;
	if (a == 0)
	{
		this->pos.x = pos.x + 1;
	}
	else
	{
		this->pos.x = pos.x - 1;
	}
	int b = rand() % 2;
	if (b == 0)
	{
		this->pos.z = pos.z + 1;
	}
	else
	{
		this->pos.z = pos.z - 1;
	}

	this->pos.y = pos.y;

	speed.x = ((rand() % 2001) - 1000) * 0.001f * 60.45f;
	speed.z = ((rand() % 2001) - 1000) * 0.001f * 60.00f;
	speed.y = ((rand() % 2001) - 1000) * 0.001f * 50.0f;



	accel.x = ((rand() % 2001) - 1000) * 0.001f * 5.4f;
	accel.y = ((rand() % 2001) - 1000) * 0.001f * 5.4f;
	accel.z = ((rand() % 2001) - 1000) * 0.001f * 5.4f;



	dispTimer = 0;
	dispMaxTimer = 27;

}

void ParticleHit::Update(float elapsedTime)
{
	int a = rand() % 2;
	if (a == 0)
	{
		speed.x += accel.x;
		speed.y -= accel.y;
		speed.z += accel.z;
	}
	else
	{
		speed.x -= accel.x;
		speed.y += accel.y;
		speed.z -= accel.z;
	}
	pos.x += speed.x*elapsedTime;
	pos.y += speed.y*elapsedTime;
	pos.z += speed.z*elapsedTime;

	scale = InSine(static_cast<float>(dispTimer), static_cast<float>(dispMaxTimer), 0.f, 1.5);
	w = OutQuad(static_cast<float>(dispTimer), static_cast<float>(dispMaxTimer), 0.f, 100);
	color.w = w;

	dispTimer++;

	if (dispTimer >= dispMaxTimer)	exist = false;
}

void ParticleTst::Init(const DirectX::XMFLOAT3& pos, std::shared_ptr<Board> board)
{
	exist = true;
	this->board = board;

	w = 1.0f;
	color = pParticleManager->Data.color;
	scale = pParticleManager->Data.scale;

	this->pos = pos;

	speed.x = ((rand() % 2001) - 1000) * 0.001f * 0.645f;
	speed.z = ((rand() % 2001) - 1000) * 0.001f * 0.400f;
	speed.y = ((rand() % 2001) - 1000) * 0.001f * 0.500f;



	accel.x = ((rand() % 2001) - 1000) * 0.001f * 0.014f;
	accel.y = ((rand() % 2001) - 1000) * 0.001f * 0.014f;
	accel.z = ((rand() % 2001) - 1000) * 0.001f * 0.011f;



	dispTimer = 0;
	dispMaxTimer = pParticleManager->Data.dispMaxTimer;
}

void ParticleTst::Update(float elapsedTime)
{

	int a = rand() % 2;
	if (a == 0)
	{
		speed.x += accel.x;
		speed.y -= accel.y;
		speed.z += accel.z;
	}
	else
	{
		speed.x -= accel.x;
		speed.y += accel.y;
		speed.z -= accel.z;
	}
	pos.x += speed.x;
	pos.y += speed.y;
	pos.z += speed.z;

	scale = InSine(static_cast<float>(dispTimer), static_cast<float>(dispMaxTimer), 0.f, pParticleManager->Data.scale);
	w = OutQuad(static_cast<float>(dispTimer), static_cast<float>(dispMaxTimer), 0.f, pParticleManager->Data.color.w);
	color.w = w;
	dispTimer++;

	if (dispTimer >= dispMaxTimer)	exist = false;
}

void ParticleCharge::Init(const DirectX::XMFLOAT3& pos, std::shared_ptr<Board> board)
{
	exist = true;
	this->board = board;

	w = 1.0f;
	color = pParticleManager->Data.color;
	scale = pParticleManager->Data.scale;

	pos2 = pos;

	int a = rand() % 201 - 101;
	int b = rand() % 201 - 101;
	this->pos.x = pos.x + a;
	this->pos.z = pos.z + b;
	/*if (a == 0)
	{
		this->pos.x = pos.x + pParticleManager->Data.plusPos.x;
	}
	else
	{
		this->pos.x = pos.x - pParticleManager->Data.plusPos.x;
	}
	int b = rand() % 2;
	if (b == 0)
	{
		this->pos.z = pos.z + pParticleManager->Data.plusPos.z;
	}
	else
	{
		this->pos.z = pos.z - pParticleManager->Data.plusPos.z;
	}
	*/
	this->pos.y = 0;

	speed = pParticleManager->Data.speed;
	accel = pParticleManager->Data.accel;

	dispTimer = 0;
	dispMaxTimer = pParticleManager->Data.dispMaxTimer;
}

void ParticleCharge::Update(float elapsedTime)
{

	DirectX::XMVECTOR pVec = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR p2Vec = DirectX::XMLoadFloat3(&pos2);//指定pos(引数)

	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(p2Vec, pVec);//周辺から指定posへのベクトル
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);

	DirectX::XMFLOAT3 d;
	DirectX::XMStoreFloat3(&d, dir);

	pos.x = pos.x + d.x * 3.0f;
	pos.z = pos.z + d.z * 3.0f;

	scale = pParticleManager->Data.scale;//InSine(static_cast<float>(dispTimer), static_cast<float>(dispMaxTimer), 0.f, pParticleManager->Data.scale);
	w = 1.0f;//OutQuad(static_cast<float>(dispTimer), static_cast<float>(dispMaxTimer), 0.f, pParticleManager->Data.color.w);
	color.w = w;
	dispTimer++;

	if (dispTimer >= dispMaxTimer)	exist = false;
}
