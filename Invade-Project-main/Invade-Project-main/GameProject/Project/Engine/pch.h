﻿// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN  
	#define FLOORHEIGET 400  //맵 2층 1층 간격
	#define MAXOBSTACLE2A 26 //2층 360도회전 장애물 개수
#define MAXOBSTACLE2B 14 //2층 시계추 장애물 개수
#define MAXOBSTACLE2H 9 //2층 허들 장애물 개수



// 여기에 미리 컴파일하려는 헤더 추가
#include <iostream>
#include <Windows.h>

#include "global.h"

#endif //PCH_H
