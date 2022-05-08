#pragma once
/*
	pch는 프로젝트 내부에서만 활용
	외부의 프로젝트에서 참조하기를 윈하는 애들을 분류하는 용도
*/

#include "Types.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"
#include "CoreMacro.h"
#include "Container.h"

#include <windows.h>
#include <iostream>
using namespace std;

#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include "Lock.h"
#include "RefCounting.h"
#include "ObjectPool.h"
#include "TypeCast.h"

#include "Memory.h"
#include "SendBuffer.h"
#include "Session.h"
#include "LockQueue.h"
