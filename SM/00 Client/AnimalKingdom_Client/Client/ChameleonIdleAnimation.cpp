#include "stdafx.h"
#include "ChameleonIdleAnimation.h"
#include "Function.h"
#include "Singleton.h"

CChameleonIdleAnimation* CSingleton<CChameleonIdleAnimation>::m_pInstance;

DWORD CChameleonIdleAnimation::Release( void )
{
	CAnimation::Release();
	delete this;

	return 0;
}

void CChameleonIdleAnimation::Enter( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos )
{
}

void CChameleonIdleAnimation::Execute( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos )
{
	fTimePos += fTimeDelta * m_fSpeed;

	if( m_dwLength <= ( DWORD )fTimePos ) fTimePos = 0.f;
}

void CChameleonIdleAnimation::Exit( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos )
{
}