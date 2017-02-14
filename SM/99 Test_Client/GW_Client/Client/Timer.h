#pragma once

#ifndef _TIMER_H_
#define _TIMER_H_

class CTimer
{
private:
	CTimer( void ) = default;
	~CTimer( void ) = default;
public:
	float	Get_TimeDelta( void );
public:
	void	Set_TimeDelta( void );
private:
	HRESULT Initialize( void );
public:
	static CTimer* Create( void );

private: // private 변수
	LARGE_INTEGER		m_FrameTime;	/* 현재 총 진동수 */
	LARGE_INTEGER		m_FixTime;		/* 지난 Tick 진동수 - 시간 계산용 */
	LARGE_INTEGER		m_LastTime;		/* 지난 Tick 진동수 - Tick 당의 진동수 갱신용 */
	LARGE_INTEGER		m_CpuTick;		/* 주기 */
	float				m_fTime;		/* 한 Tick 당 시간 */
public:
	void	Release( void );
};

#endif