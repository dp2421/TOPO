#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define FLOORHEIGET 400

//�ӽ� Ŀ���� ����Ŭ����
class Vec3
{
public:
	float x;
	float y;
	float z;

	Vec3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	Vec3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

#ifndef ������ �����ε�

	friend Vec3 operator+(const Vec3& a, const Vec3& b) // ���� ����
	{
		float Vx = a.x + b.x;
		float Vy = a.y + b.y;
		float Vz = a.z + b.z;

		return Vec3(Vx, Vy, Vz);
	}
	friend Vec3 operator-(const Vec3& a, const Vec3& b) // ���� �E��
	{
		float Vx = a.x - b.x;
		float Vy = a.y - b.y;
		float Vz = a.z - b.z;

		return Vec3(Vx, Vy, Vz);
	}
	friend float operator*(const Vec3& a, const Vec3& b) //���� ���ϱ�(��Į�� ��)
	{
		float Vx = a.x * b.x;
		float Vy = a.y * b.y;
		float Vz = a.z * b.z;

		return Vx + Vy + Vz;
	}
	bool operator==(Vec3 a) // �� ���Ͱ� ������
	{
		if (x == a.x && y == a.y && z == a.z)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

#endif // ������ �����ε�

	static float InnerCos(Vec3 v1, Vec3 v2) //���� ���ϱ�
	{
		float Cos = (v1 * v2) / ((v1.Magnitude() * v2.Magnitude()));

		return Cos;
	}
	Vec3 Normalrize() // ����ȭ(�������ͷ� �����)
	{
		Vec3 normal;
		float magnitude = this->Magnitude();
		normal.x = x / magnitude;
		normal.y = y / magnitude;
		normal.z = z / magnitude;

		return normal;
	}
	float Magnitude() // ���� ũ��
	{
		auto magnitude = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

		return magnitude;
	}
	static double Seta(float cos) //��Ÿ�� ���ϱ� ����(��׸�)
	{
		return acos(cos) * (180 / 3.14);
	}

	static Vec3 Proj(Vec3 v1, Vec3 v2) // ����(normal) * ũ��(magnitude = Proj(V1)
	{
		Vec3 result;

		result.x = ((v1 * v2) / (v2 * v2)) * v2.x;
		result.y = ((v1 * v2) / (v2 * v2)) * v2.y;
		result.z = ((v1 * v2) / (v2 * v2)) * v2.z;

		return result;
	}
	static  Vec3 Orthogonal_Projection(Vec3 v1, Vec3 proj) // ����ȭ
	{
		Vec3 ortho;
		ortho = v1 - proj;
		return ortho;
	}
	static Vec3 CrossProduct(Vec3 v1, Vec3 v2) //����
	{
		Vec3 cross;
		cross.x = v1.y * v2.z - v1.z * v2.y;
		cross.y = v1.z * v2.x - v1.x - v2.z;
		cross.z = v1.x * v2.y - v1.y * v2.x;

		return cross;
	}
};

enum class OBSTACLE_STATE {
	STOP,
	MOVEA,
	MOVEB,
};

// ������Ʈ ������ ��� ����ü
struct ObstacleObject
{
   // string name;
	OBSTACLE_STATE state;
	Vec3 vPos;
};

void writeObstacleDate()
{
	vector<ObstacleObject> obstacles;

	// obstacles ��ü�� ��ֹ� ���� �߰�
	// 360�� ��ֹ�
	ObstacleObject tempObstacle;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = -1; k <= 1; k += 2)
			{
				tempObstacle.state = OBSTACLE_STATE::MOVEA;
				tempObstacle.vPos = Vec3((1280.f + 280.f * i + 640.f * j), 10.f, 1200.f + 400.f * i);
			}
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			tempObstacle.state = OBSTACLE_STATE::MOVEA;
			tempObstacle.vPos = Vec3(-1600.f + 600.f * i + 640.f, 10.f, 8960.f + 1280.f * j);
			obstacles.push_back(tempObstacle);
		}
	}
	for (int i = 0; i < 2; ++i)
	{
		tempObstacle.state = OBSTACLE_STATE::MOVEA;
		tempObstacle.vPos = Vec3(500.f - i * (500.f), 10.f, 21000.f);
		obstacles.push_back(tempObstacle);
	}
	//1��
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			tempObstacle.state = OBSTACLE_STATE::MOVEA;
			tempObstacle.vPos = Vec3(-1120.f + 560.f * i, 10.f - FLOORHEIGET, 8400.f + 1600.f * j);
			obstacles.push_back(tempObstacle);
		}
	}
	for (int i = 0; i < 2; ++i)
	{
		tempObstacle.state = OBSTACLE_STATE::MOVEA;
		tempObstacle.vPos = Vec3(-2560.f + 480.f * i, 10.f - FLOORHEIGET, 14720.f);
		obstacles.push_back(tempObstacle);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	//�ð��� ��ֹ�
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			tempObstacle.state = OBSTACLE_STATE::MOVEB;
			tempObstacle.vPos = Vec3(-840.f + 840.f * i, 330.f, 8400.f + 1200.f * j);
			obstacles.push_back(tempObstacle);
		}
	}
	for (int i = 0; i < 2; ++i)
	{
		tempObstacle.state = OBSTACLE_STATE::MOVEB;
		tempObstacle.vPos = Vec3(-1680.f + 1680.f * i, 330.f, 14800.f);
		obstacles.push_back(tempObstacle);
	}
	//1��
	for (int i = 0; i < 3; ++i)
	{
		tempObstacle.state = OBSTACLE_STATE::MOVEB;
		tempObstacle.vPos = Vec3(-640.f - 640.f * i, 330.f - FLOORHEIGET, 12800.f + 600.f * i);
		obstacles.push_back(tempObstacle);
	}
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			tempObstacle.state = OBSTACLE_STATE::MOVEB;
			tempObstacle.vPos = Vec3(-480.f + 960.f * i, 330.f - FLOORHEIGET, 16800.f + 800.f * j);
			obstacles.push_back(tempObstacle);
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//��� ��ֹ�
	for (int i = 0; i < 3; ++i)
	{
		tempObstacle.state = OBSTACLE_STATE::STOP;
		tempObstacle.vPos = Vec3(-160.f + 160.f * i, 80.f, 21000.f);
		obstacles.push_back(tempObstacle);
	}
	for (int i = 0; i < 2; ++i)
	{
		for (int j = -1; j <= 1; j += 2)
		{
			tempObstacle.state = OBSTACLE_STATE::STOP;
			tempObstacle.vPos = Vec3(j * (800.f + 160.f * i), 80.f, 21000.f);
			obstacles.push_back(tempObstacle);
		}
	}
	//1��
	for (int i = 0; i < 17; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			tempObstacle.state = OBSTACLE_STATE::STOP;
			tempObstacle.vPos = Vec3(-1280.f + 160.f * i, 80.f - FLOORHEIGET, 9160.f + 1680.f * j);
			obstacles.push_back(tempObstacle);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	// ���̳ʸ� ���Ϸ� ����
	ofstream outFile("obstacles.bin", ios::out | ios::binary);
	if (!outFile) {
		std::cerr << "Failed to open obstacles.bin" << std::endl;
		return;
	}

	for (int i = 0; i < obstacles.size(); i++) {
		ObstacleObject obstacle = obstacles[i];
		outFile.write(reinterpret_cast<char*>(&obstacle), sizeof(obstacle));
	}
	outFile.close();
}


void readObstaclesDate()
{
	vector<ObstacleObject> obstacles;

	// ���̳ʸ� ���� �б�
	ifstream inFile("obstacles.bin", std::ios::in | std::ios::binary);

	if (!inFile) {
		std::cerr << "Failed to open obstacles.bin" << std::endl;
		return;
	}

	while (!inFile.eof()) {
		ObstacleObject obstacle;
		inFile.read(reinterpret_cast<char*>(&obstacle), sizeof(obstacle));
		obstacles.push_back(obstacle);
	}
	inFile.close();

	////�׽�Ʈ
	//for (ObstacleObject i : obstacles)
	//	cout << i.vPos.x << endl;
}

int main()
{
	//writeObstacleDate();
	readObstaclesDate();
	return 0;
}