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

class Obstacle
{
public:
	Obstacle(ObstacleInfo info);
	~Obstacle();

	ObstacleInfo data;
	float rotate = 0;
	float deltaRotate;
	Vector3 position;
	vector<Collider> collider;
};

