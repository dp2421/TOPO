#pragma once

using namespace DirectX;

class Collider
{
public:
    Collider();
    ~Collider();

    XMFLOAT3 center;
    XMFLOAT3 extents;
    XMFLOAT4 orientation; // Quaternion

    bool isCollisionOBB(Collider& rhs);
    bool isCollisionAABB(Collider& rhs);

    void Slide(XMVECTOR& velocity, const XMVECTOR& normal);
};

