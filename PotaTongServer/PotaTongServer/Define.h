#pragma once

#define SafeDelete(p) if(p != nullptr) delete p; p = nullptr;

#define ClientException(client, id) if (client.second->ID == id) continue;\
								if (client.second->ID == -1) continue;

#define DistanceVec3(a, b) sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));

constexpr int MatchingTime = 5;
constexpr int GameTime = 10;
constexpr int SurviveTime = 60;
constexpr int Frame = 30;
constexpr auto PushTime = 1500ms;
constexpr auto DeltaTimeMilli = chrono::duration_cast<chrono::milliseconds>(chrono::duration<float>(1.f / Frame));
constexpr auto DeltaTimeNano = chrono::duration_cast<chrono::nanoseconds>(chrono::duration<float>(1.f / Frame));
constexpr auto DeltaTimefloat = chrono::duration<float>(1.f / Frame);

constexpr int RacingMAX = 24;
constexpr int SurviveMAX = 8;

constexpr float SPEED = 1000.f;
constexpr float JUMPVEL = 450.0f;
constexpr float GRAVITY = 9.8f*2.f;
constexpr float PUSHDISTANCE = 150.f;

const Vector3 PlayerStartPos{ -1800, 10, 35 };
const Vector3 MeteoStartPos{ -450, 10, -450 };
const Vector3 JumpStartPos{ -850, 10, 850 };

const float PlayerStartDistance = 200.0f;
const Vector3 MeteoStartDistance{ 50, 0, 50 };
const Vector3 JumpStartDistance{ 200, 0, 0 };

const Vector3 PlayerCollider{ 100.0f / 2, 160.0f / 2, 75.0f / 2 };
const Vector3 PlayerOffset{ 0.0f, 90.0f, 0.0f };

const Vector3 TileCollider{ 100.0f / 2, 100.0f / 2, 100.0f / 2 };
const Vector3 TileOffset{ 0.0f, 0.0f, 0.0f };

const Vector3 RotateObsRotateBodyCollider{ 450.0f / 2, 65.0f / 2, 80.0f / 2};
const Vector3 RotateObsRotateBodyOffset{ 0.0f, 152.0f, 0.0f };
const Vector3 RotateObsPillarCollider{ 80.0f / 2, 230.0f / 2, 80.0f / 2};
const Vector3 RotateObsPillarOffset{ 0.0f, 115.0f, 0.0f };

const Vector3 PendulumObsCollider{ 100.0f / 2, 300.0f / 2, 100.0f / 2 };
const Vector3 PendulumObsOffset{ 0.0f, -160.0f, 0.0f };

const Vector3 HurdleObsCollider{ 140.0f  /2 , 20.0f /2, 50.0f /2};
const Vector3 HurdleumObsOffset{ 0.0f, 0.0f, 0.0f };

