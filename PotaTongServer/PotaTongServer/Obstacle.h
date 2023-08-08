#pragma once

enum class OBSTACLE_STATE {
	STOP,
	SPIN,
	PENDULUM,
};

struct ObstacleInfo
{
	// string name;
	OBSTACLE_STATE state;
	float x;
	float y;
	float z;
};

struct JumpObstacleObject
{
	OBSTACLE_STATE state;
	float x, y, z;
	float xScale, yScale, zScale;
};

class Obstacle
{
public:
	Obstacle(ObstacleInfo info, int i);
	Obstacle(JumpObstacleObject info);
	~Obstacle();

	ObstacleInfo data;
	JumpObstacleObject jData;
	int index;
	float rotate = 0;
	float deltaRotate;
	float angularVelocity;
	Vector3 position;
	vector<Collider> collider;
};

