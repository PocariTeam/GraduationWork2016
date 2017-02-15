#include "stdafx.h"
#include "ThreadMgr.h"
#include "Functor.h"
#include "TextureMgr.h"
#include "MeshMgr.h"
#include "ShaderMgr.h"
#include "AnimateMeshMgr.h"

CThreadMgr* CSingleton<CThreadMgr>::m_pInstance;

HRESULT CThreadMgr::Initialize( HWND hWnd )
{
	m_dwThreadCnt = 0;
	m_dwMaxThreadCnt = GetProcessorCnt();
	m_dwMaxThreadCnt *= 2;

	m_hThread = new HANDLE[ m_dwMaxThreadCnt ];
	m_hBeginEvent = new HANDLE[ m_dwMaxThreadCnt ];
	m_hEndEvent = new HANDLE[ m_dwMaxThreadCnt ];

	m_hWnd = hWnd;

	return S_OK;
}

void CThreadMgr::ClearThreadlist( void )
{
	m_dwThreadCnt = 0;
	ZeroMemory( m_hThread, sizeof( HANDLE ) * m_dwMaxThreadCnt );
	ZeroMemory( m_hBeginEvent, sizeof( HANDLE ) * m_dwMaxThreadCnt );
	ZeroMemory( m_hEndEvent, sizeof( HANDLE ) * m_dwMaxThreadCnt );

	for_each( m_mapThread.begin(), m_mapThread.end(), DeletePair() );
}

DWORD CThreadMgr::Release( void )
{
	ClearThreadlist();
	delete[] m_hThread;
	m_hThread = nullptr;
	delete[] m_hBeginEvent;
	m_hBeginEvent = nullptr;
	delete[] m_hEndEvent;
	m_hEndEvent = nullptr;

	delete this;

	return 0;
}

HRESULT CThreadMgr::Create_Thread( eThread_Type eType, const char* pThreadKey, const char* pLoadPath, ID3D11Device* pDevice, UINT dwLoadType )
{
	// map 컨테이너에 동일한 키값 있는지 검사

	switch( eType )
	{
	case THREAD_RENDER :
		return E_FAIL;
	case THREAD_LOAD :
		Create_LoadThread( pThreadKey, pLoadPath, pDevice, dwLoadType );
		break;
	}

	return S_OK;
}

HRESULT CThreadMgr::Create_Thread( eThread_Type eType, const char* pThreadKey, ID3D11DeviceContext* pContext, UINT dwRenderType )
{
	return S_OK;
}

bool CThreadMgr::Check_EndEvent( DWORD dwTimeOut/* = INFINITE*/ )
{
	DWORD dwSignal = WaitForMultipleObjects( m_dwThreadCnt, m_hEndEvent, TRUE, dwTimeOut );

	if( WAIT_OBJECT_0 == dwSignal ) return true;
	return false;
}

UINT CThreadMgr::GetProcessorCnt( void )
{
	unsigned int iCnt{ 0 };

	SYSTEM_INFO		tSystemInfo;
	GetSystemInfo( &tSystemInfo );
	iCnt = tSystemInfo.dwNumberOfProcessors;

	return iCnt;
}

HRESULT CThreadMgr::Create_LoadThread( const char* pThreadKey, const char* pLoadPath, ID3D11Device* pDevice, UINT dwLoadType )
{
	auto iter_Find = m_mapThread.find( pThreadKey );

	if( iter_Find != m_mapThread.end() )
	{
		MessageBox( nullptr, "Already Exist\n", nullptr, MB_OK );
		return E_FAIL;
	}

	THREAD_LOADING*	 pThread = new THREAD_LOADING;
	pThread->m_pDevice = pDevice;
	pThread->m_hThread = CreateThread( NULL, 0, LoadResources, ( LPVOID ) pThread, 0, nullptr );
	pThread->m_hBeginEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	pThread->m_hEndEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	pThread->m_dwType = dwLoadType;
	strcpy_s( pThread->m_strPath, MAX_PATH, pLoadPath );

	m_hThread[ m_dwThreadCnt ] = pThread->m_hThread;
	m_hBeginEvent[ m_dwThreadCnt ] = pThread->m_hThread;
	m_hEndEvent[ m_dwThreadCnt ] = pThread->m_hThread;

	m_mapThread.insert( make_pair( pThreadKey, pThread ) );
	m_dwThreadCnt++;

	return S_OK;
}

DWORD WINAPI CThreadMgr::LoadResources( LPVOID lpParameter )
{
	THREAD_LOADING*	pParam = ( THREAD_LOADING* )lpParameter;

	switch( pParam->m_dwType )
	{
	case LOAD_TEXTURE:
		CTextureMgr::GetInstance()->Load( pParam->m_pDevice, pParam->m_strPath );
		break;
	case LOAD_MESH:
		CMeshMgr::GetInstance()->Load( pParam->m_pDevice, pParam->m_strPath );
		break;
	case LOAD_SHADER:
		CShaderMgr::GetInstance()->Load( pParam->m_pDevice, pParam->m_strPath );
		break;
	case LOAD_ANIMATEMESH:
		CAnimateMeshMgr::GetInstance()->Load( pParam->m_pDevice, pParam->m_strPath );
		break;
	}

	ResetEvent( pParam->m_hEndEvent );
	return 0;
}
