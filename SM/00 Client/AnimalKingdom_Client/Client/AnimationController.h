#pragma once

#ifndef AnimationController_h__
#define AnimationController_h__

#include "Enum.h"
#include "Base.h"

template<class CHAR_TYPE>
class CAnimation;
template<class CHAR_TYPE>
class CAnimationController
	: public CBase
{
private:
	explicit CAnimationController( CHAR_TYPE* pOwner );
	// explicit CAnimationController( const CAnimationController& pAnimationController );
	~CAnimationController();
public:
	void Update( const float& fTimeDelta );
	CAnimationController* Clone( void );
private:
	CHAR_TYPE*	m_pOwner;

	CAnimation<CHAR_TYPE>*	m_pCurrentAnimation;
	CAnimation<CHAR_TYPE>*	m_pPreviousAnimation;
	CAnimation<CHAR_TYPE>*	m_pGlobalAnimation;
public:
	void	SetCurrentAnimation( CAnimation<CHAR_TYPE>* pAnimation );
	void	SetGlobalAnimation( CAnimation<CHAR_TYPE>* pAnimation );
	void	SetPreviousAnimation( CAnimation<CHAR_TYPE>* pAnimation );
	void	Change_Animation( CAnimation<CHAR_TYPE>* pAnimation );
	void	RevertToPreviousAnimation( void );
};

#endif // AnimationController_h__
