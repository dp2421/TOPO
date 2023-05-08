#include "pch.h"
#include "Collider.h"

Collider::Collider()
{
}

Collider::~Collider()
{

}

XMVECTOR Collider::XMVectorMin(XMVECTOR v)
{
    XMVECTOR minV = v;
    minV = DirectX::XMVectorMin(minV, XMVectorSwizzle<1, 0, 3, 2>(minV));
    minV = DirectX::XMVectorMin(minV, XMVectorSwizzle<2, 3, 0, 1>(minV));
    return minV;
}

XMVECTOR Collider::GetClosestFaceNormal(const BoundingBox& box, FXMVECTOR point)
{
	XMVECTOR boxCenter = XMLoadFloat3(&box.Center);
	XMVECTOR vectorFromCenter = XMVectorSubtract(point, boxCenter);

	XMVECTOR distances = XMVectorSelect(
		XMVectorSubtract(point, XMLoadFloat3(&box.Extents)), // Minimum distances
		XMVectorSubtract(XMLoadFloat3(&box.Extents), point), // Maximum distances
		XMVectorGreater(vectorFromCenter, XMVectorZero()));         // Choose the right sign

	XMVECTOR closestDistance = XMVectorMin(distances);
	int faceIndex = XMVectorGetIntX(XMVectorEqual(distances, closestDistance));

	switch (faceIndex)
	{
	case 0: // Left face
		return XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	case 1: // Right face
		return XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	case 2: // Bottom face
		return XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	case 3: // Top face
		return XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	case 4: // Back face
		return XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	case 5: // Front face
		return XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	default:
		return XMVectorZero();
	}
}

BoundingBox Collider::getBoundingbox()
{
	return BoundingBox
	{
		(*this->position + this->offset).ConvertXMFLOAT3(),
		this->size.ConvertXMFLOAT3(),
	};
}

bool Collider::isCollisionOBB(Collider& rhs)
{
	BoundingOrientedBox thisBox
	{ 
		(*this->position + this->offset).ConvertXMFLOAT3(),
		this->size.ConvertXMFLOAT3(),
		this->orientation
	};

	BoundingOrientedBox rhsBox
	{ 
		(*rhs.position + rhs.offset).ConvertXMFLOAT3(), 
		rhs.size.ConvertXMFLOAT3(),
		rhs.orientation
	};

	return thisBox.Intersects(rhsBox);
}

bool Collider::isCollisionAABB(Collider& rhs)
{
	BoundingBox thisBox
	{
		(*this->position + this->offset).ConvertXMFLOAT3(),
		this->size.ConvertXMFLOAT3(),
	};

	BoundingBox rhsBox
	{
		(*rhs.position + rhs.offset).ConvertXMFLOAT3(),
		rhs.size.ConvertXMFLOAT3(),
	};

	return thisBox.Intersects(rhsBox);
}

void Collider::Slide(XMVECTOR& velocity, const XMVECTOR& normal)
{
}
