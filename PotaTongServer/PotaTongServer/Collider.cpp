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
	XMVECTOR minValues = XMVectorSplatX(v);
	minValues = DirectX::XMVectorMin(minValues, XMVectorSplatY(v));
	minValues = DirectX::XMVectorMin(minValues, XMVectorSplatZ(v));
	return minValues;
}

XMVECTOR Collider::GetClosestFaceNormal(const BoundingBox& box, FXMVECTOR point)
{
	XMVECTOR boxCenter = XMLoadFloat3(&box.Center);
	XMVECTOR boxExtents = XMLoadFloat3(&box.Extents); 
	XMVECTOR vectorFromCenter = point - boxCenter;
	XMVECTOR distances = XMVectorSubtract(XMVectorAbs(vectorFromCenter), boxExtents);

	XMVECTOR closestDistance = XMVectorMin(distances);

	int faceIndex = -1;
	float minDistanceX = XMVectorGetX(distances);
	float minDistanceY = XMVectorGetY(distances);
	float minDistanceZ = XMVectorGetZ(distances);

	if (minDistanceX < minDistanceY && minDistanceX < minDistanceZ)
		faceIndex = 0; // X face
	else if (minDistanceY < minDistanceZ)
		faceIndex = 1; // Y face
	else
		faceIndex = 2; // Z face

	cout << distances.m128_f32[0] << " "
		<< distances.m128_f32[1] << " "
		<< distances.m128_f32[2] << " "
		<< faceIndex << endl;

	switch (faceIndex)
	{
	case 0: // x
		cout << "LEFT" << endl;
		return DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	case 1: // y
		cout << "BOTTOM" << endl;
		return DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	case 2: // z
		cout << "BACK" << endl;
		return DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	default:
		return DirectX::XMVectorZero();
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
	XMFLOAT4 thisOrientation;
	XMFLOAT4 rhsOrientation;

	XMStoreFloat4(&thisOrientation, this->orientation);
	XMStoreFloat4(&rhsOrientation, rhs.orientation);

	BoundingOrientedBox thisBox
	{ 
		(*this->position + this->offset).ConvertXMFLOAT3(),
		this->size.ConvertXMFLOAT3(),
		thisOrientation
	};

	BoundingOrientedBox rhsBox
	{ 
		(*rhs.position + rhs.offset).ConvertXMFLOAT3(), 
		rhs.size.ConvertXMFLOAT3(),
		rhsOrientation
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
