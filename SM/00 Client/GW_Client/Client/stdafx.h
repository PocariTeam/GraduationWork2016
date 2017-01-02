// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#define NOMINMAX
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include <vector>
#include <list>
#include <map>
#include <string>
#include <fstream>

#include <d3d11.h>
#include <d3dx11.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#ifdef _DEBUG
#pragma comment( lib, "d3dx11d.lib" )
#pragma comment( lib, "dxErr.lib" )
#else
#pragma comment( lib, "d3dx11.lib" )
#endif

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "D3DCompiler.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "dInput8.lib" )

#pragma comment( lib, "NxCharacter64.lib" )
#pragma comment( lib, "PhysXCooking64.lib" )
#pragma comment( lib, "PhysXCore64.lib" )
#pragma comment( lib, "PhysXLoader64.lib" )

using namespace std;
using namespace DirectX;