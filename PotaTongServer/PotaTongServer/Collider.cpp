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
	BoundingOrientedBox thisBox{ this->center, this->extents, this->orientation };
	BoundingOrientedBox rhsBox{ rhs.center, rhs.extents, rhs.orientation };

	return thisBox.Intersects(rhsBox);
}

bool Collider::isCollisionAABB(Collider& rhs)
{
	BoundingBox thisBox{ this->center, this->extents };
	BoundingBox rhsBox{ rhs.center, rhs.extents };

	return thisBox.Intersects(rhsBox);
}

void Collider::Slide(XMVECTOR& velocity, const XMVECTOR& normal)
{
}
