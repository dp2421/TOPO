#include "pch.h"
#include "Obstacle.h"

Obstacle::Obstacle(ObstacleInfo info):
	data(info)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> urd(0.1f, 5.0f);
	deltaRotate = urd(gen);

	position = Vector3(data.x, data.y, data.z);

	switch (data.state)
	{
	case OBSTACLE_STATE::STOP:
	{
		Collider coll;
		coll.offset = HurdleumObsOffset;
		coll.size = HurdleObsCollider;
		coll.position = &position;
		collider.push_back(coll);
		break;
	}
	case OBSTACLE_STATE::SPIN:
	{
		Collider coll;
		coll.offset = RotateObsPillarOffset;
		coll.size = RotateObsPillarCollider;
		coll.position = &position;
		collider.push_back(coll);

		coll.offset = RotateObsRotateBodyOffset;
		coll.size = RotateObsRotateBodyCollider;
		coll.position = &position;
		collider.push_back(coll);
		break;
	}
	case OBSTACLE_STATE::PENDULUM:
	{
		Collider coll;
		coll.offset = PendulumObsOffset;
		coll.size = PendulumObsCollider;
		coll.position = &position;
		collider.push_back(coll);
		break;
	}
	default:
		break;
	}
}

Obstacle::~Obstacle()
{

}


