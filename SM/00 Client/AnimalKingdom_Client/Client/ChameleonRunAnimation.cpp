#include "stdafx.h"
#include "ChameleonRunAnimation.h"
#include "Function.h"
#include "Singleton.h"

CChameleonRunAnimation* CSingleton<CChameleonRunAnimation>::m_pInstance;

DWORD CChameleonRunAnimation::Release( void )
{
	CAnimation::Release();
	delete this;

	return 0;
}

void CChameleonRunAnimation::Enter( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos )
{
}

void CChameleonRunAnimation::Execute( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos )
{
}

void CChameleonRunAnimation::Exit( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos )
{
}