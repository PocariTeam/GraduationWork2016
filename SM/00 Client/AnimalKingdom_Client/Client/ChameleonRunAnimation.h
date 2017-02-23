#pragma once

#ifndef ChameleonRunAnimation_h__
#define ChameleonRunAnimation_h__

#include "Animation.h"
#include "Singleton.h"

class CGameObject;
class CChameleonRunAnimation
	: public CAnimation, public CSingleton<CChameleonRunAnimation>
{
public:
	DWORD	Release( void );
public:
	void Enter( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos );
	void Execute( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos );
	void Exit( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos );
};

#endif // ChameleonRunAnimation_h__