#pragma once
#include <iostream>
#include <mutex>
#include <thread>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <vector>
#include <concurrent_unordered_map.h>
#include <functional>
#include <queue>
#include <algorithm>
#include <concurrent_priority_queue.h>
#include "../../Protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

#include "Vector3.h"

#include "Event.h"
#include "ThreadSafeQueue.h"

#include "OverlappedEx.h"
#include "Client.h"

#include "MatchingManager.h"
#include "GameMode.h"

#include "RacingMode.h"
#include "SurviveMode.h"
#include "MeteorMode.h"
#include "BombMode.h"
#include "ObatacleMode.h"

#include "ServerBase.h"