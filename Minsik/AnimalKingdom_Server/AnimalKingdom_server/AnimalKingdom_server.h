#pragma once
#include "stdafx.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Winmm.lib")

#pragma comment( lib, "NxCharacter64.lib" )
#pragma comment( lib, "PhysXCooking64.lib" )
#pragma comment( lib, "PhysXCore64.lib" )
#pragma comment( lib, "PhysXLoader64.lib" )

#pragma comment( lib, "d3dx11d.lib" )

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define NOMINMAX

#include <Ws2tcpip.h>
#include <winsock2.h>
#include <mswsock.h>
#include <Mmsystem.h>
#include <Ws2spi.h>
#include <Mstcpip.h>

#include <windows.h>
#include <iostream>
#include <io.h>
#include <cstdlib>
#include <stdio.h>
#include <cstdint>

#include <assert.h> 
#include <fcntl.h>
#include <algorithm>  
#include <functional>

#include <thread>
#include <mutex>
#include <memory>

#include <string>
#include <tchar.h>

#include <ctime>
#include <random>
#include <typeinfo>    //typeinfo

#include <DirectXMath.h>
using namespace DirectX;

// TODO: 공용 매크로
//#define CONTEXT_SWITCH     std::this_thread::sleep_for(std::chrono::nanoseconds(1))
#if _DEBUG
#define CONTEXT_SWITCH	 Sleep(1)
#else
#define CONTEXT_SWITCH		::SwitchToThread()
#endif

typedef void(*Function)(void *);

//기타 유틸
//#include "./Util/csv_parser/csv_parser.hpp"
#include "./Util/tinyXml/tinyxml.h"

// TODO: 필수 헤더 파일
//-------------------------------------------------------------------//
////프로토콜
#include "./Protocol/Protocol.h"

#include "./Util/Type.h"
#include "./Util/Util.h"

#include "./Util/GameObject.h"
#include "./Util/Singleton.h"
#include "./Util/RandomMT.h"

#include "./Util/Clock.h"
#include "./Util/Logger.h"
#include "./Util/Assert.h"
//
#include "./Util/Table.h"
#include "./Util/Thread.h"
#include "./Util/Lock.h"
#include "./Util/ThreadJobQueue.h"
#include "./Util/Task.h"
//
#include "./Util/MemoryLeak.h"
#include "./Util/Memory_LowFragmentationHeap.h"
#include "./Util/Minidump.h"
//
#include "./Util/Config.h"
//
///컨텐츠
//
#include "./Contents/RoomManager.h"
#include "./Contents/GameRoom.h"
#include "./Contents/Player.h"
#include "./Contents/PacketManager.h"
#include "./PhysX/PhysXManager.h"
//
////서버
#include "./Net/IOCPServer.h"
#include "./Net/SessionManager.h"
#include "./Net/Session.h"
//#include "./Net/SessionMonitor.h"
//
//


// 전역 변수
#include "./main/Shutdown.h"
