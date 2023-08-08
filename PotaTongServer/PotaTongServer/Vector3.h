#pragma once
class Vector3
{
public:
	Vector3(float xx, float yy, float zz);
	Vector3();
	Vector3(const Vector3& vec);

    Vector3 operator+(const Vector3& vec) const;
    Vector3 operator+(float val) const;
    Vector3& operator+=(const Vector3& vec);
    Vector3& operator+=(float val);

    Vector3 operator-(const Vector3& vec) const;
    Vector3 operator-(float val) const;
    Vector3& operator-=(const Vector3& vec);
    Vector3& operator-=(float val);

    Vector3 operator*(const Vector3& vec) const;
    Vector3 operator*(float factor) const;
    Vector3& operator*=(const Vector3& vec);
    Vector3& operator*=(float factor);

    Vector3 operator/(const Vector3& vec) const;
    Vector3 operator/(float div) const;
    Vector3& operator/=(const Vector3& vec);
    Vector3& operator/=(float div);

    Vector3& operator=(const Vector3& vec);

    void Normalize();

    float Length() const;

    DirectX::XMFLOAT3 ConvertXMFLOAT3();
    DirectX::XMFLOAT4 ConvertXMFLOAT4();

    bool operator==(const Vector3& vec) const;
    bool operator!=(const Vector3& vec) const;
public:
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };

        float elements[3];
    };

    static Vector3 Up()
    {
        return Vector3(0.0f, 1.0f, 0.0f);
    }

    static Vector3 Forward()
    {
        return Vector3(0.0f, 0.0f, 1.0f);
    }

    static Vector3 Right()
    {
        return Vector3(1.0f, 0.0f, 0.0f);
    }

    static Vector3 Zero()
    {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    static Vector3 One()
    {
        return Vector3(1.0f, 1.0f, 1.0f);
    }

    static float Distance(const Vector3 lhs, const Vector3 rhs)
    {
        return sqrtf(powf((rhs.x - lhs.x), 2) + pow((rhs.y - lhs.y), 2) + pow((rhs.z - lhs.z), 2));
    }

    static Vector3 Cross(const Vector3& vec1, const Vector3& vec2)
    {
        return Vector3(
            vec1.y * vec2.z - vec1.z * vec2.y,
            vec1.z * vec2.x - vec1.x * vec2.z,
            vec1.x * vec2.y - vec1.y * vec2.x
        );
    }

    static float Dot(const Vector3& vec1, const Vector3& vec2)
    {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }
};

