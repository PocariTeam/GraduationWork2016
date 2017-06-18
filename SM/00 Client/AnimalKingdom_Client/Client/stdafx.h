// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#define NOMINMAX
#include <windows.h>

// C 런타임 헤더 파일입니다.
#ifdef _DEBUG
	// Physics 헤더와 충돌 발생
	// #define _CRTDBG_MAP_ALLOC
	#include <stdlib.h> 
	#include <crtdbg.h>
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#else
	#include <stdlib.h> 
#endif  // _DEBUG

#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include <vector>
#include <list>
#include <map>
#include <queue>
#include <string>
#include <fstream>
#include <algorithm>
#include <chrono>
#include "Protocol.h"

#include <d3d11.h>
#include <d3dx11.h>

//#include <DirectXMath.h>
//#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#define DIRECTINPUT_VERSION	0x0800

#include <winsock2.h>
#include <dinput.h>

#ifdef _DEBUG
#pragma comment( lib, "../Executable/ThirdParty/DX11/lib/d3dx11d.lib" )
#pragma comment( lib, "../Executable/ThirdParty/DX11/lib/dxErr.lib" )
#else
#pragma comment( lib, "../Executable/ThirdParty/DX11/lib/d3dx11.lib" )
#endif

#pragma comment( lib, "../Executable/ThirdParty/DX11/lib/d3d11.lib" )
#pragma comment( lib, "../Executable/ThirdParty/DX11/lib/D3DCompiler.lib" )
#pragma comment( lib, "../Executable/ThirdParty/DX11/lib/dxgi.lib" )
#pragma comment( lib, "../Executable/ThirdParty/DX11/lib/dxguid.lib" )
#pragma comment( lib, "../Executable/ThirdParty/DX11/lib/dInput8.lib" )

#pragma comment( lib, "../Executable/ThirdParty/Physics/SDKs/lib/win64/NxCharacter64.lib" )
#pragma comment( lib, "../Executable/ThirdParty/Physics/SDKs/lib/win64/PhysXCooking64.lib" )
#pragma comment( lib, "../Executable/ThirdParty/Physics/SDKs/lib/win64/PhysXCore64.lib" )
#pragma comment( lib, "../Executable/ThirdParty/Physics/SDKs/lib/win64/PhysXLoader64.lib" )

#pragma comment( lib, "ws2_32" )

using namespace std;
using namespace std::chrono;
using namespace DirectX;