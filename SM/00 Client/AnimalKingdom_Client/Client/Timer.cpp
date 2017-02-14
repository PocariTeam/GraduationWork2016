#include "stdafx.h"
#include "Timer.h"

float CTimer::GetTimeDelta( void )
{
	return m_fTime;
}

void CTimer::Calculate_TimeDelta( void )
{
	/* �������� ���� */
	QueryPerformanceCounter( &m_FrameTime );

	/* 1�ʴ� ������( �ֱ� )���� ���� �������� ���� ���� �������� ���̰� ũ�ٸ� -> �� 1�ʴ� �������� ��ȭ�� ���� */
	if( m_FrameTime.QuadPart - m_LastTime.QuadPart > m_CpuTick.QuadPart )
	{
		/* ���� 1�ʴ� ������( �ֱ� )�� ���Ѵ� */
		QueryPerformanceFrequency( &m_CpuTick );
		/* ���� �������� ����ϴ� ������ ���� �������� ���� */
		m_LastTime.QuadPart = m_FrameTime.QuadPart;
	}

	/* �ð� = ( ���� ������ - ���� ������ ) / �ֱ� */
	m_fTime = float( m_FrameTime.QuadPart - m_FixTime.QuadPart ) / m_CpuTick.QuadPart;

	m_FixTime = m_FrameTime;
}

HRESULT CTimer::Initialize( void )
{
	/* ó�� Timer �� �߰��� ���� �������� �־��� */
	QueryPerformanceCounter( &m_FrameTime );
	QueryPerformanceCounter( &m_FixTime );
	QueryPerformanceCounter( &m_LastTime );

	/* �ֱ�( 1�ʴ� ������ )�� ��� */
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