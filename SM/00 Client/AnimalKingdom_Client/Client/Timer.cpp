#include "stdafx.h"
#include "Timer.h"

float CTimer::GetTimeDelta( void )
{
	return m_fTime;
}

void CTimer::Calculate_TimeDelta( void )
{
	/* 진동수를 갱신 */
	QueryPerformanceCounter( &m_FrameTime );

	/* 1초당 진동수( 주기 )보다 지난 진동수와 현재 현재 진동수의 차이가 크다면 -> 한 1초당 진동수의 변화를 갱신 */
	if( m_FrameTime.QuadPart - m_LastTime.QuadPart > m_CpuTick.QuadPart )
	{
		/* 새로 1초당 진동수( 주기 )를 구한다 */
		QueryPerformanceFrequency( &m_CpuTick );
		/* 지난 진동수를 기록하는 변수에 현재 진동수를 대입 */
		m_LastTime.QuadPart = m_FrameTime.QuadPart;
	}

	/* 시간 = ( 현재 진동수 - 지난 진동수 ) / 주기 */
	m_fTime = float( m_FrameTime.QuadPart - m_FixTime.QuadPart ) / m_CpuTick.QuadPart;

	m_FixTime = m_FrameTime;
}

HRESULT CTimer::Initialize( void )
{
	/* 처음 Timer 를 추가할 때의 진동수를 넣어줌 */
	QueryPerformanceCounter( &m_FrameTime );
	QueryPerformanceCounter( &m_FixTime );
	QueryPerformanceCounter( &m_LastTime );

	/* 주기( 1초당 진동수 )를 기록 */
	QueryPerformanceFrequency( &m_CpuTick );

	return S_OK;
}

CTimer* CTimer::Create( void )
{
	CTimer* pTimer = new CTimer;

	if( FAILED( pTimer->Initialize() ) )
	{
		pTimer->Release();
		pTimer = nullptr;
		return nullptr;
	}

	return pTimer;
}

DWORD  CTimer::Release( void )
{
	delete this;

	return 0;
}