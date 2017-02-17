#include "stdafx.h"
#include "AnimationMgr.h"
#include "AnimationController.h"
#include <fstream>
#include "Functor.h"

CAnimationMgr*	CSingleton<CAnimationMgr>::m_pInstance;

template<class CHAR_TYPE>
CAnimationController<CHAR_TYPE>* CAnimationMgr::Clone( const eCharacter_Type& eType )
{
	CAnimationController<CHAR_TYPE>*	pController = Find( eType );
	if( nullptr == pController ) return nullptr;

	return pController->Clone();
}

template<class CHAR_TYPE>
CAnimationController<CHAR_TYPE>* CAnimationMgr::Find( const eCharacter_Type& eType )
{
	if( eType < CHARACTER_END )
		return m_vecAnimationController[ eType ];
	return nullptr;
}

HRESULT CAnimationMgr::Load( ID3D11Device* pDevice, const char* pFilePath )
{
	return S_OK;
}

DWORD CAnimationMgr::Release( void )
{
	return 0;
}
