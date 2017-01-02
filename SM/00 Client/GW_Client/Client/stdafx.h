// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#define NOMINMAX
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

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