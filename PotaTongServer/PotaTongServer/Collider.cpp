#include "pch.h"
#include "Collider.h"

Collider::Collider()
{
}

Collider::~Collider()
{

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
	DirectX::XMFLOAT3 test = (*this->position + this->offset).ConvertXMFLOAT3();
	BoundingBox thisBox
	{
		test,
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
