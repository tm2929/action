#include "Vector.h"


VECTOR2& VECTOR2::operator=(const VECTOR2& v)
{
    x = v.x; y = v.y; return *this;
}

VECTOR2& VECTOR2::operator+=(const VECTOR2& v)
{
    x += v.x;
    y += v.y;
    return *this;
}

VECTOR2& VECTOR2::operator-=(const VECTOR2& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}


VECTOR2& VECTOR2::operator*=(float f)
{
    x *= f;
    y *= f;
    return *this;
}

VECTOR2& VECTOR2::operator/=(float f)
{
    x /= f;
    y /= f;
    return *this;
}


VECTOR2 VECTOR2::operator+() const
{
    return VECTOR2(x, y);
}


VECTOR2 VECTOR2::operator-() const
{
    return VECTOR2(-x, -y);
}


VECTOR2 VECTOR2::operator+(const VECTOR2& v) const
{
    return VECTOR2(x + v.x, y + v.y);
}


VECTOR2 VECTOR2::operator-(const VECTOR2& v) const
{
    return VECTOR2(x - v.x, y - v.y);
}


VECTOR2 VECTOR2::operator*(float f) const
{
    return VECTOR2(x * f, y * f);
}


VECTOR2 operator*(float f, const VECTOR2& v)
{
    return VECTOR2(v.x * f, v.y * f);
}

VECTOR3 operator*(float, const VECTOR3&)
{
    return VECTOR3();
}


VECTOR2 VECTOR2::operator/(float f) const
{
    return VECTOR2(x / f, y / f);
}


bool VECTOR2::operator == (const VECTOR2& v) const
{
    return (x == v.x) && (y == v.y);
}


bool VECTOR2::operator != (const VECTOR2& v) const
{
    return (x != v.x) || (y != v.y);
}


//float Vec2LengthSq(const VECTOR2& v)
//{
//    return v.x * v.x + v.y * v.y;
//}


float Vec2Length(const VECTOR2& v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}


const VECTOR2& Vec2Normalize(const VECTOR2& v, VECTOR2& out)
{
    float d = Vec2Length(v);
    if (d == 0.0f) return v;

    float f = 1 / d;
    out = v;
    out.x *= f;
    out.y *= f;
    return out;
}

VECTOR3& VECTOR3::operator=(const VECTOR3& v)
{
    x = v.x; y = v.y; z = v.z; return *this;
}

VECTOR3& VECTOR3::operator+=(const VECTOR3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

VECTOR3& VECTOR3::operator-=(const VECTOR3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}


VECTOR3& VECTOR3::operator*=(float f)
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

VECTOR3& VECTOR3::operator/=(float f)
{
    x /= f;
    y /= f;
    z /= f;
    return *this;
}


VECTOR3 VECTOR3::operator+() const
{
    return VECTOR3(x, y, z);
}


VECTOR3 VECTOR3::operator-() const
{
    return VECTOR3(-x, -y, -z);
}


VECTOR3 VECTOR3::operator+(const VECTOR3& v) const
{
    return VECTOR3(x + v.x, y + v.y, z + v.z);
}


VECTOR3 VECTOR3::operator-(const VECTOR3& v) const
{
    return VECTOR3(x - v.x, y - v.y, z - v.z);
}

VECTOR3 VECTOR3::operator*(const VECTOR3& v) const
{
    return VECTOR3(x * v.x, y * v.y, z * v.z);
}

VECTOR3 VECTOR3::operator*(float f) const
{
    return VECTOR3(x * f, y * f, z * f);
}

VECTOR3 VECTOR3::operator/(float f) const
{
    return VECTOR3(x / f, y / f, z / f);
}


bool VECTOR3::operator == (const VECTOR3& v) const
{
    return (x == v.x) && (y == v.y) && (z == v.z);
}


bool VECTOR3::operator != (const VECTOR3& v) const
{
    return (x != v.x) || (y != v.y) || (z != v.z);
}

float Vec3Length(const VECTOR3& v)
{
    return sqrtf(Vec3Dot(v, v));
}

float Vec3DistVecter(const VECTOR3& v1, const VECTOR3& v2)
{
    return (v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y) + (v2.z - v1.z) * (v2.z - v1.z);
}

VECTOR3 CreateVec3(const VECTOR3& v1, const VECTOR3& v2)
{
    return v2 - v1;
}

void Vec3Normalize(VECTOR3* out, const VECTOR3& v)
{
    float d = Vec3Length(v);
    if (d == 0.0f)
    {
        *out = v;
        return;
    }

    float f = 1 / d;
    VECTOR3 tmp = v;
    tmp *= f;
    *out = tmp;
}

float Vec3Dot(const VECTOR3& v1, const VECTOR3& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

void Vec3Cross(VECTOR3* out, const VECTOR3& v1, const VECTOR3& v2)
{
    VECTOR3 v;
    v.x = v1.y * v2.z - v1.z * v2.y;
    v.y = v1.z * v2.x - v1.x * v2.z;
    v.z = v1.x * v2.y - v1.y * v2.x;
    *out = v;
}



VECTOR4& VECTOR4::operator=(const VECTOR4& v)
{
    x = v.x; y = v.y; z = v.z, w = v.w; return *this;
}

VECTOR4& VECTOR4::operator+=(const VECTOR4& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

VECTOR4& VECTOR4::operator-=(const VECTOR4& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}


VECTOR4& VECTOR4::operator*=(float f)
{
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return *this;
}

VECTOR4& VECTOR4::operator/=(float f)
{
    x /= f;
    y /= f;
    z /= f;
    w /= f;
    return *this;
}


VECTOR4 VECTOR4::operator+() const
{
    return VECTOR4(x, y, z, w);
}


VECTOR4 VECTOR4::operator-() const
{
    return VECTOR4(-x, -y, -z, -w);
}


VECTOR4 VECTOR4::operator+(const VECTOR4& v) const
{
    return VECTOR4(x + v.x, y + v.y, z + v.z, w + v.w);
}


VECTOR4 VECTOR4::operator-(const VECTOR4& v) const
{
    return VECTOR4(x - v.x, y - v.y, z - v.z, w - v.w);
}


VECTOR4 VECTOR4::operator*(float f) const
{
    return VECTOR4(x * f, y * f, z * f, w * f);
}

VECTOR4 VECTOR4::operator*(const VECTOR4& v) const
{
    return VECTOR4(x * v.x, y * v.y, z * v.z, w * v.w);
}

VECTOR4 VECTOR4::operator/(float f) const
{
    return VECTOR4(x / f, y / f, z / f, w / f);
}


bool VECTOR4::operator == (const VECTOR4& v) const
{
    return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w);
}


bool VECTOR4::operator != (const VECTOR4& v) const
{
    return (x != v.x) || (y != v.y) || (z != v.z) || (w != v.w);
}
