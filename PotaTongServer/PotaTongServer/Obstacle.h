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
	ObstacleInfo data;

	XMFLOAT3 position;
};

