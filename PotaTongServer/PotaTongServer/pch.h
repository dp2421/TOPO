#pragma once
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <vector>
#include <concurrent_unordered_map.h>
#include <functional>
#include <queue>
#include <algorithm>
#include <chrono>
#include <concurrent_priority_queue.h>
#include <DirectXMath.h>
#include <directxcollision.h>
#include "../../Protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;
#include "Vector3.h"

#include "Define.h"

#include "OverlappedEx.h"
#include "Event.h"
#include "ThreadSafeQueue.h"
#include "Collider.h"

#include "Client.h"
#include "Obstacle.h"
#include "Tile.h"

#include "MatchingManager.h"
#include "GameMode.h"

#include "RacingMode.h"
#include "SurviveMode.h"
#include "MeteorMode.h"
#include "BombMode.h"
#include "ObatacleMode.h"

#include "ServerBase.h"