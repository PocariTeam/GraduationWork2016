#pragma once

#ifndef ThreadMgr_h__
#define ThreadMgr_h__

#include "Singleton.h"
#include "Struct.h"
#include "Enum.h"

class CThreadMgr
	: public CSingleton<CThreadMgr>
{
public:
	enum eThread_Type { THREAD_LOAD, THREAD_RENDER, THREAD_END };
	using THREAD_TYPE = eThread_Type;

	enum eLoad_Type { LOAD_TEXTURE = 10, LOAD_MESH, LOAD_SHADER, LOAD_STATIC, LOAD_DYNAMIC, LOAD_ANIMATION, LOAD_END };
	using LOAD_TYPE = eLoad_Type;
public:
	HRESULT Initialize( HWND hWnd );
	void  ClearThreadlist( void );
	DWORD Release( void );
	// Load Thread Create
	HRESULT Create_Thread( eThread_Type eType, const char* pThreadKey, const char* pLoadPath, ID3D11Device* pDevice, UINT dwLoadType );
	// Render Thread Create
	HRESULT Create_Thread( eThread_Type eType, const char* pThreadKey, ID3D11DeviceContext* pContext, UINT dwRenderType );
public:
	bool	Check_EndEvent( DWORD dwTimeOut = INFINITE );

private:
	UINT	GetProcessorCnt( void );
	HRESULT Create_LoadThread( const char* pThreadKey, const char* pLoadPath, ID3D11Device* pDevice, UINT dwLoadType );
private:
	static DWORD WINAPI	LoadResources( LPVOID lpParameter );
private:
	map<const string, THREAD_INFO*>	m_mapThread;
	HWND							m_hWnd;
	DWORD							m_dwThreadCnt;
	DWORD							m_dwMaxThreadCnt;
	HANDLE*							m_hThread;
	HANDLE*							m_hBeginEvent;
	HANDLE*							m_hEndEvent;
};

#endif // ThreadMgr_h__
