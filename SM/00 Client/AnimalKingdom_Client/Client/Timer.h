#pragma once

#ifndef _TIMER_H_
#define _TIMER_H_

class CTimer
{
private:
	CTimer( void ) = default;
	~CTimer( void ) = default;
public:
	double	GetTimeDelta( void );
public:
	void	Calculate_TimeDelta( void );
private:
	HRESULT Initialize( void );
public:
	static CTimer* Create( void );

private: // private ����
	LARGE_INTEGER		m_FrameTime;	/* ���� �� ������ */
	LARGE_INTEGER		m_FixTime;		/* ���� Tick ������ - �ð� ���� */
	LARGE_INTEGER		m_LastTime;		/* ���� Tick ������ - Tick ���� ������ ���ſ� */
	LARGE_INTEGER		m_CpuTick;		/* �ֱ� */
	double				m_dTime;		/* �� Tick �� �ð� */
public:
	DWORD	Release( void );
};

#endif