#pragma once

using namespace DirectX;

class Collider
{
public:
    Collider();
    ~Collider();

    Vector3* position;

    Vector3 offset;
    Vector3 size;
    XMFLOAT4 orientation; // Quaternion

    bool isCollisionOBB(Collider& rhs);
    bool isCollisionAABB(Collider& rhs);

    void Slide(XMVECTOR& velocity, const XMVECTOR& normal);
};

