#pragma once
#include <iostream>
#include <mutex>
#include <thread>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <vector>
#include <concurrent_unordered_map.h>
#include <queue>
#include "../../Protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

#include "ThreadSafeQueue.h"

#include "OverlappedEx.h"
#include "ServerBase.h"
#include "MatchingManager.h"

#include "Client.h"

#include "GameMode.h"

#include "RacingMode.h"
#include "SurviveMode.h"
#include "MeteorMode.h"
#include "BombMode.h"
#include "ObatacleMode.h"