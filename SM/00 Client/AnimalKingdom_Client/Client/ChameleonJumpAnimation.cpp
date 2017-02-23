#include "stdafx.h"
#include "ChameleonJumpAnimation.h"
#include "Function.h"
#include "Singleton.h"

CChameleonJumpAnimation* CSingleton<CChameleonJumpAnimation>::m_pInstance;

DWORD CChameleonJumpAnimation::Release( void )
{
	CAnimation::Release();
	delete this;

	return 0;
}

void CChameleonJumpAnimation::Enter( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos )
{
}

void CChameleonJumpAnimation::Execute( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos )
{
}

void CChameleonJumpAnimation::Exit( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos )
{
}