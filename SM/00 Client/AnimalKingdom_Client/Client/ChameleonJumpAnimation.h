#pragma once

#ifndef ChameleonJumpAnimation_h__
#define ChameleonJumpAnimation_h__

#include "Animation.h"
#include "Singleton.h"

class CGameObject;
class CChameleonJumpAnimation
	: public CAnimation, public CSingleton<CChameleonJumpAnimation>
{
public:
	DWORD	Release( void );
public:
	void Enter( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos );
	void Execute( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos );
	void Exit( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos );
};

#endif // ChameleonJumpAnimation_h__