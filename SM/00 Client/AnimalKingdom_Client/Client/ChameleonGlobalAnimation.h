#pragma once
#ifndef ChameleonGlobalAnimation_h__
#define ChameleonGlobalAnimation_h__

#include "Animation.h"

class CGameObject;
class CChameleonGlobalAnimation
	: public CAnimation, public CSingleton<CChameleonGlobalAnimation>
{
public:
	DWORD	Release( void );
public:
	HRESULT Load( const char* pFilePath );
	void Enter( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos );
	void Execute( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos );
	void Exit( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos );
};

#endif // ChameleonGlobalAnimation_h__