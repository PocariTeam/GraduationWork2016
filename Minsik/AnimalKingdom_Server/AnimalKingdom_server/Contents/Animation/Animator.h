#pragma once

#ifndef Animator_h__
#define Animator_h__


#include "Base.h"

class Player;
class CAnimationSet;
class CAnimator
	: public CBase
{
private:
	explicit CAnimator( void );
	explicit CAnimator( const CAnimator& Instance );
public:
	void Update( Player* pOwner, const float& fTimeDelta );
	CAnimator* Clone( void );
	static CAnimator* Create( const char* pFilePath );
	DWORD Release( void );
private:
	HRESULT	Initialize( const char* pFilePath );
	HRESULT Load( const char* pFilePath );
	void	ConnectActorShape( Player* pOwner );
public:
	HRESULT Add( STATE eState, const char* pFilePath );
	void	Change_Animation( STATE eState );
	bool	GetCurrentAnimationFinished();
	float	GetPerFinish();
private:
	CAnimationSet*			m_pCurrentAnimationSet;
	CAnimationSet*			m_pPreviousAnimationSet;

	vector<CAnimationSet*>	m_vecAnimationSet;
	string*					m_pArrJointName;
	DWORD					m_dwJointCnt;
	float					m_fTimePos;
};

#endif // AnimationController_h__
