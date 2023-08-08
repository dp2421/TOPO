#include "pch.h"
#include "Obstacle.h"

Obstacle::Obstacle(ObstacleInfo info, int i):
	data(info),
	index(i)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> urd(0.1f, 5.0f);
	deltaRotate = urd(gen);
	angularVelocity = deltaRotate * Frame;

	position.x = data.x;
	position.y = data.y;
	position.z = data.z;

	switch (data.state)
	{
	case OBSTACLE_STATE::STOP:
	{
		Collider coll;
		coll.offset = HurdleumObsOffset;
		coll.size = HurdleObsCollider;
		collider.push_back(coll);
		collider[0].position = new Vector3(data.x, data.y, data.z);
		index = 0;
		break;
	}
	case OBSTACLE_STATE::SPIN:
	{
		Collider coll;
		coll.offset = RotateObsPillarOffset;
		coll.size = RotateObsPillarCollider;
		collider.push_back(coll);
		collider[0].position = new Vector3(data.x, data.y, data.z);

		coll.offset = RotateObsRotateBodyOffset;
		coll.size = RotateObsRotateBodyCollider;
		collider.push_back(coll);
		collider[1].position = new Vector3(data.x, data.y, data.z);
		break;
	}
	case OBSTACLE_STATE::PENDULUM:
	{
		Collider coll;
		coll.offset = PendulumObsOffset;
		coll.offset.y += 150;
		coll.size = PendulumObsCollider;
		coll.size.y += 150;
		collider.push_back(coll);
		collider[0].position = new Vector3(data.x, data.y, data.z);
		break;
	}
	default:
		break;
	}
}

Obstacle::Obstacle(JumpObstacleObject info)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> urd(3.0f, 5.0f);
	deltaRotate = urd(gen);
	angularVelocity = deltaRotate * Frame;

	position.x = data.x;
	position.y = data.y;
	position.z = data.z;

	auto vec = Vector3(info.xScale, info.yScale, info.zScale);

	Collider coll;
	coll.offset = RotateObsPillarOffset;
	coll.size = RotateObsPillarCollider;
	collider.push_back(coll);
	collider[0].position = new Vector3(data.x, data.y, data.z);

	coll.offset = RotateObsRotateBodyOffset;
	coll.size = RotateObsRotateBodyCollider * vec;
	collider.push_back(coll);
	collider[1].position = new Vector3(data.x, data.y, data.z);
}

Obstacle::~Obstacle()
{

}


