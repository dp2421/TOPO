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
    XMVECTOR orientation{ 0,0,0,1 }; // Quaternion

    XMVECTOR XMVectorMin(XMVECTOR v);
    XMVECTOR GetClosestFaceNormal(const BoundingBox& box, FXMVECTOR point);
    BoundingBox getBoundingbox();
    BoundingOrientedBox getBoundingOrientedBox();

    bool isCollisionOBB(Collider& rhs);
    bool isCollisionAABB(Collider& rhs);

    void Slide(XMVECTOR& velocity, const XMVECTOR& normal);
};

