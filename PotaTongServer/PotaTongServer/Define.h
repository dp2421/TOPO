#pragma once

#define ClientException(client, id) if (client.second->ID == id) continue;\
								if (client.second->ID == -1) continue;

constexpr int Frame = 30;
constexpr auto DeltaTimeMilli = chrono::duration_cast<chrono::milliseconds>(chrono::duration<float>(1.f / Frame));
constexpr auto DeltaTimeNano = chrono::duration_cast<chrono::nanoseconds>(chrono::duration<float>(1.f / Frame));
constexpr auto DeltaTimefloat = chrono::duration<float>(1.f / Frame);

constexpr float SPEED = 200.f;
constexpr float JUMPVEL = 300.0f;
constexpr float GRAVITY = 200.0f;
