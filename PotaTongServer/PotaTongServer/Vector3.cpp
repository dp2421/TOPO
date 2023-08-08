#include "pch.h"
#include "Vector3.h"

Vector3::Vector3(float xx, float yy, float zz) :
	x(xx),
	y(yy),
	z(zz)
{
}

Vector3::Vector3() :
	Vector3(0.0f, 0.0f, 0.0f)
{
}

Vector3::Vector3(const Vector3& vec) :
	Vector3(vec.x, vec.y, vec.z)
{
}

Vector3 Vector3::operator+(const Vector3& vec) const
{
    return Vector3(x + vec.x,
        y + vec.y,
        z + vec.z);
}

Vector3 Vector3::operator+(float val) const
{
    return Vector3(x + val,
        y + val,
        z + val);
}

Vector3& Vector3::operator+=(const Vector3& vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return (*this);
}

Vector3& Vector3::operator+=(float val)
{
    x += val;
    y += val;
    z += val;
    return (*this);
}

Vector3 Vector3::operator-(const Vector3& vec) const
{
    return Vector3(x - vec.x,
        y - vec.y,
        z - vec.z);
}

Vector3 Vector3::operator-(float val) const
{
    return Vector3(x - val,
        y - val,
        z - val);
}

Vector3& Vector3::operator-=(const Vector3& vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    return (*this);
}

Vector3& Vector3::operator-=(float val)
{
    x -= val;
    y -= val;
    z -= val;
    return (*this);
}

Vector3 Vector3::operator*(const Vector3& vec) const
{
    return Vector3(x * vec.x,
        y * vec.y,
        z * vec.z);
}

Vector3 Vector3::operator*(float factor) const
{
    return Vector3(x * factor,
        y * factor,
        z * factor);
}

Vector3& Vector3::operator*=(const Vector3& vec)
{
    x *= vec.x;
    y *= vec.y;
    z *= vec.z;
    return (*this);
}

Vector3& Vector3::operator*=(float factor)
{
    x *= factor;
    y *= factor;
    z *= factor;
    return (*this);
}

Vector3 Vector3::operator/(const Vector3& vec) const
{
    return Vector3(x / vec.x,
        y / vec.y,
        z / vec.z);
}

Vector3 Vector3::operator/(float div) const
{
    float factor = 1.0f / div;
    return Vector3(x * factor,
        y * factor,
        z * factor);
}

Vector3& Vector3::operator/=(const Vector3& vec)
{
    x /= vec.x;
    y /= vec.y;
    z /= vec.z;
    return (*this);
}

Vector3& Vector3::operator/=(float div)
{
    float factor = 1.0f / div;
    x *= factor;
    y *= factor;
    z *= factor;
    return (*this);
}

Vector3& Vector3::operator=(const Vector3& vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
    return (*this);
}

DirectX::XMFLOAT3 Vector3::ConvertXMFLOAT3()
{
    return DirectX::XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT4 Vector3::ConvertXMFLOAT4()
{
    return DirectX::XMFLOAT4(x, y, z, 1);
}

bool Vector3::operator==(const Vector3& vec) const
{
    return (x == vec.x) && (y == vec.y) && (z == vec.z);
}

bool Vector3::operator!=(const Vector3& vec) const
{
    return (x != vec.x) || (y != vec.y) || (z != vec.z);
}