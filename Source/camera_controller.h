#pragma once
#include <windows.h>
#include <DirectXMath.h>

class CameraController
{
public:
	CameraController();

	void Update();

	const DirectX::XMFLOAT3& GetEye() const { return m_eye; }
	const DirectX::XMFLOAT3& GetFocus() const { return m_focus; }
	const DirectX::XMFLOAT3& GetUp() const { return m_up; }

private:
	DirectX::XMFLOAT3	m_eye = DirectX::XMFLOAT3(0, 0, -200.0f);
	DirectX::XMFLOAT3	m_focus = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3	m_up = DirectX::XMFLOAT3(0, 1, 0);
	DirectX::XMFLOAT3	m_right = DirectX::XMFLOAT3(1, 0, 0);
	float				m_distance = 0.0f;

	DirectX::XMFLOAT2	m_old_cursor;
	DirectX::XMFLOAT2	m_new_cursor;

	float				m_rotate_x = 0.0f;
	float				m_rotate_y = 0.0f;
};
