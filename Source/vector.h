#pragma once
#include <directxmath.h>

class VECTOR2 : public DirectX::XMFLOAT2
{
public:
	VECTOR2() : DirectX::XMFLOAT2(0, 0) {}
	VECTOR2(float x, float y) : DirectX::XMFLOAT2(x, y) {}
	VECTOR2(const VECTOR2& v) { x = v.x; y = v.y; }
	~VECTOR2() {}

	VECTOR2& operator=(const VECTOR2&);
	VECTOR2& operator+=(const VECTOR2&);
	VECTOR2& operator-=(const VECTOR2&);
	VECTOR2& operator*=(float);
	VECTOR2& operator/=(float);

	VECTOR2 operator+() const;
	VECTOR2 operator-() const;

	VECTOR2 operator+(const VECTOR2&) const;
	VECTOR2 operator-(const VECTOR2&) const;
	VECTOR2 operator*(float) const;
	friend VECTOR2 operator*(float, const VECTOR2&);
	VECTOR2 operator/(float) const;

	bool operator == (const VECTOR2&) const;
	bool operator != (const VECTOR2&) const;
};


//------< プロトタイプ宣言 >-----------------------------------------------------

float Vec2LengthSq(const VECTOR2&);
float Vec2Length(const VECTOR2&);
const VECTOR2& Vec2Normalize(const VECTOR2&, VECTOR2&);

class VECTOR3 : public DirectX::XMFLOAT3
{
public:
	VECTOR3() : DirectX::XMFLOAT3(0, 0, 0) {}
	VECTOR3(float x, float y, float z) : DirectX::XMFLOAT3(x, y, z) {}
	VECTOR3(DirectX::XMFLOAT3 a) : DirectX::XMFLOAT3(a.x, a.y, a.z) {}
	~VECTOR3() {}

	VECTOR3& operator=(const VECTOR3&);
	VECTOR3& operator+=(const VECTOR3&);
	VECTOR3& operator-=(const VECTOR3&);
	VECTOR3& operator*=(float);
	VECTOR3& operator/=(float);

	VECTOR3 operator+() const;
	VECTOR3 operator-() const;

	VECTOR3 operator+(const VECTOR3&) const;
	VECTOR3 operator-(const VECTOR3&) const;
	VECTOR3 operator*(const VECTOR3&) const;

	VECTOR3 operator*(float) const;
	friend VECTOR3 operator*(float, const VECTOR3&);
	VECTOR3 operator/(float) const;

	bool operator == (const VECTOR3&) const;
	bool operator != (const VECTOR3&) const;
};

VECTOR3 CreateVec3(const VECTOR3& v1, const VECTOR3& v2);
float Vec3Length(const VECTOR3& pV);
float Vec3DistVecter(const VECTOR3& v1, const VECTOR3& v2);
void Vec3Normalize(VECTOR3* out, const VECTOR3& v);
float Vec3Dot(const VECTOR3& v1, const VECTOR3& v2);
void Vec3Cross(VECTOR3* out, const VECTOR3& v1, const VECTOR3& v2);

class VECTOR4 : public DirectX::XMFLOAT4
{
public:
	VECTOR4() : DirectX::XMFLOAT4(0, 0, 0, 0) {}
	VECTOR4(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w) {}
	~VECTOR4() {}

	VECTOR4& operator=(const VECTOR4&);
	VECTOR4& operator+=(const VECTOR4&);
	VECTOR4& operator-=(const VECTOR4&);
	VECTOR4& operator*=(float);
	VECTOR4& operator/=(float);

	VECTOR4 operator+() const;
	VECTOR4 operator-() const;

	VECTOR4 operator+(const VECTOR4&) const;
	VECTOR4 operator-(const VECTOR4&) const;
	VECTOR4 operator*(float) const;
	friend VECTOR4 operator*(float, const VECTOR4&);
	VECTOR4 operator*(const VECTOR4&) const;
	VECTOR4 operator/(float) const;

	bool operator == (const VECTOR4&) const;
	bool operator != (const VECTOR4&) const;
};
class UVECTOR4 : public DirectX::XMUINT4
{
public:
	UVECTOR4() : DirectX::XMUINT4(0, 0, 0, 0) {};
	~UVECTOR4() {};

	UVECTOR4(const int x, const int y, const int z, const int w) : DirectX::XMUINT4(x, y, z, w) {};
	UVECTOR4(const UVECTOR4& v) { x = v.x; y = v.y; z = v.z; w = v.z; }

	UVECTOR4& operator=(const UVECTOR4& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
	UVECTOR4& operator+=(const UVECTOR4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	UVECTOR4& operator-=(const UVECTOR4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	UVECTOR4& operator*=(const int f) { x *= f; y *= f; z *= f; w *= f; return *this; }
	UVECTOR4& operator/=(const int f) { x /= f; y /= f; z /= f; w /= f; return *this; }

	UVECTOR4 operator+() const { return UVECTOR4(x, y, z, w); }
	//UVECTOR4 operator-() const { return UVECTOR4(-x, -y, -z, -w); }

	UVECTOR4 operator+(const UVECTOR4& v) const { return UVECTOR4(x + v.x, y + v.y, z + v.z, w + v.w); }
	UVECTOR4 operator-(const UVECTOR4& v) const { return UVECTOR4(x - v.x, y - v.y, z - v.z, w - v.w); }
	UVECTOR4 operator*(const int f) const { return UVECTOR4(x * f, y * f, z * f, w * f); }
	UVECTOR4 operator/(const int f) const { return UVECTOR4(x / f, y / f, z / f, w * f); }

	bool operator == (const UVECTOR4& v) const { return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w); }
	bool operator != (const UVECTOR4& v) const { return (x != v.x) || (y != v.y) || (z != v.z) || (w != v.w); }
};
class FLOAT4X4 : public DirectX::XMFLOAT4X4
{
public:
	FLOAT4X4() : DirectX::XMFLOAT4X4() {};
	~FLOAT4X4() {};

	FLOAT4X4(const float _11, const float _12, const float _13, const float _14,
		const float _21, const float _22, const float _23, const float _24,
		const float _31, const float _32, const float _33, const float _34,
		const float _41, const float _42, const float _43, const float _44)
	{
		this->_11 = _11; this->_12 = _12; this->_13 = _13; this->_14 = _14;
		this->_21 = _21; this->_22 = _22; this->_23 = _23; this->_24 = _24;
		this->_31 = _31; this->_32 = _32; this->_33 = _33; this->_34 = _34;
		this->_41 = _41; this->_42 = _42; this->_43 = _43; this->_44 = _44;
	}

	FLOAT4X4(const FLOAT4X4& f4x4)
	{
		_11 = f4x4._11; _12 = f4x4._12; _13 = f4x4._13; _14 = f4x4._14;
		_21 = f4x4._21; _22 = f4x4._22; _23 = f4x4._23; _24 = f4x4._24;
		_31 = f4x4._31; _32 = f4x4._32; _33 = f4x4._33; _34 = f4x4._34;
		_41 = f4x4._41; _42 = f4x4._42; _43 = f4x4._43; _44 = f4x4._44;
	}

	FLOAT4X4 operator*(const FLOAT4X4& m) const {
		float x = _11;
		float y = _12;
		float z = _13;
		float w = _14;

		FLOAT4X4 result;
		result._11 = (m._11 * x) + (m._21 * y) + (m._31 * z) + (m._41 * w);
		result._12 = (m._12 * x) + (m._22 * y) + (m._32 * z) + (m._42 * w);
		result._13 = (m._13 * x) + (m._23 * y) + (m._33 * z) + (m._43 * w);
		result._14 = (m._14 * x) + (m._24 * y) + (m._34 * z) + (m._44 * w);

		x = _21;
		y = _22;
		z = _23;
		w = _24;
		result._21 = (m._11 * x) + (m._21 * y) + (m._31 * z) + (m._41 * w);
		result._22 = (m._12 * x) + (m._22 * y) + (m._32 * z) + (m._42 * w);
		result._23 = (m._13 * x) + (m._23 * y) + (m._33 * z) + (m._43 * w);
		result._24 = (m._14 * x) + (m._24 * y) + (m._34 * z) + (m._44 * w);

		x = _31;
		y = _32;
		z = _33;
		w = _34;
		result._31 = (m._11 * x) + (m._21 * y) + (m._31 * z) + (m._41 * w);
		result._32 = (m._12 * x) + (m._22 * y) + (m._32 * z) + (m._42 * w);
		result._33 = (m._13 * x) + (m._23 * y) + (m._33 * z) + (m._43 * w);
		result._34 = (m._14 * x) + (m._24 * y) + (m._34 * z) + (m._44 * w);

		x = _41;
		y = _42;
		z = _43;
		w = _44;
		result._41 = (m._11 * x) + (m._21 * y) + (m._31 * z) + (m._41 * w);
		result._42 = (m._12 * x) + (m._22 * y) + (m._32 * z) + (m._42 * w);
		result._43 = (m._13 * x) + (m._23 * y) + (m._33 * z) + (m._43 * w);
		result._44 = (m._14 * x) + (m._24 * y) + (m._34 * z) + (m._44 * w);

		return result;
	}
};

class UINT4 : public DirectX::XMUINT4
{
public:
	UINT4() {}
	~UINT4() {}

	UINT4(const uint32_t x, const uint32_t y, const uint32_t z, const uint32_t w)
	{
		this->x = x; this->y = y; this->z = z; this->w = w;
	}

	UINT4& operator=(const UINT4& UInt4)
	{
		x = UInt4.x; y = UInt4.y; z = UInt4.z; w = UInt4.w; return *this;
	}
};

