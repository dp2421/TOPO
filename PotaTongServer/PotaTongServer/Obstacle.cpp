#include "pch.h"
#include "Obstacle.h"

Obstacle::Obstacle(ObstacleInfo info):
	data(info)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> urd(0.1f, 5.0f);
	deltaRotate = urd(gen);
}

Obstacle::~Obstacle()
{

}


