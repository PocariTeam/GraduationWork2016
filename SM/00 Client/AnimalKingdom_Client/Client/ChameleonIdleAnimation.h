#pragma once

#ifndef ChameleonIdleAnimation_h__
#define ChameleonIdleAnimation_h__

#include "Animation.h"
#include "Singleton.h"

class CGameObject;
class CChameleonIdleAnimation
	: public CAnimation, public CSingleton<CChameleonIdleAnimation>
{
public:
	DWORD	Release( void );
public:
	void Enter( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos );
	void Execute( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos );
	void Exit( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos );
};

#endif // ChameleonIdleAnimation_h__