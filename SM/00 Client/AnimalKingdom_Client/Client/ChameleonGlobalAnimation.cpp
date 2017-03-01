#include "stdafx.h"
#include "ChameleonGlobalAnimation.h"
#include "Singleton.h"

CChameleonGlobalAnimation* CSingleton<CChameleonGlobalAnimation>::m_pInstance;

DWORD CChameleonGlobalAnimation::Release( void )
{
	delete this;

	return 0;
}

HRESULT CChameleonGlobalAnimation::Load( const char* pFilePath )
{
	return S_OK;
}

void CChameleonGlobalAnimation::Enter( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos )
{
}

void CChameleonGlobalAnimation::Execute( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos )
{
}

void CChameleonGlobalAnimation::Exit( CGameObject* pChameleon, const float& fTimeDelta, float& fTimePos )
{
}
