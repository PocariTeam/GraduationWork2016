#pragma once

#ifndef AnimationSet_h__
#define AnimationSet_h__

#include "Base.h"

class CAnimationSet
	: public CBase
{
private:
	explicit CAnimationSet( void );
	explicit CAnimationSet( const CAnimationSet& Instance );
public:
	static CAnimationSet* Create( const char* pFilePath );
	CAnimationSet* Clone() { return new CAnimationSet( *this ); }
	HRESULT Load( const char* pFilePath );
	void	GetAnimationMatrix( XMFLOAT4X4* pOut );
	void	ResetTimePos( void ) { m_fTimePos = 0.f; }
	DWORD	Release( void );
	void	Update( const float& fTimeDelta );
	bool	GetFinished( void ) { return ( !m_bLoop ) & ( m_fTimePos == m_dwLength ); }
	bool	GetLoop( void ) { return m_bLoop; }
	float	GetPerFinish( void ) { return m_fTimePos / ( float )m_dwLength; }
private:
	XMFLOAT4X4**	m_dpArrFrame;
	DWORD			m_dwJointCnt;
	DWORD			m_dwLength;
	float			m_fTimePos;
	float			m_fSpeed;
	bool			m_bLoop;
};

#endif // Animation_h__