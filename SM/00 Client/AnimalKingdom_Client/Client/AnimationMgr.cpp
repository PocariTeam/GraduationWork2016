#include "stdafx.h"
#include "AnimationMgr.h"
#include "AnimationController.h"
#include <fstream>
#include "Functor.h"
#include "ChameleonGlobalAnimation.h"
#include "ChameleonIdleAnimation.h"
#include "ChameleonRunAnimation.h"
#include "ChameleonJumpAnimation.h"

CAnimationMgr*	CSingleton<CAnimationMgr>::m_pInstance;

CAnimationController* CAnimationMgr::Clone( const eCharacter_Type& eType, CGameObject* pOwner )
{
	CAnimationController*	pController = Find( eType );
	if( nullptr == pController ) return nullptr;

	return pController->Clone( pOwner );
}

CAnimationController* CAnimationMgr::Find( const eCharacter_Type& eType )
{
	if( eType < CHARACTER_END )
		return m_vecAnimationController[ eType ];
	return nullptr;
}

HRESULT CAnimationMgr::Load( ID3D11Device* pDevice, const char* pFilePath )
{
	m_vecAnimationController.reserve( ( size_t )CHARACTER_END );

	ifstream pIn{ pFilePath };
	int iAllCnt{}, iCharactor{}, iState{};
	bool bJoint{};
	char szPath[ MAX_PATH ]{""};
	
	pIn >> iAllCnt;
	
	for( int i = 0; i < iAllCnt; ++i )
	{
		pIn >> iCharactor;
		pIn >> bJoint;
		pIn >> iState;
		pIn >> szPath;

		if( bJoint )
			Add( pDevice, szPath );
		else
			if( FAILED( Add( ( CHARACTER_TYPE )iCharactor, ( STATE )iState, szPath ) ) )
				return E_FAIL;
	}

	return S_OK;
}

DWORD CAnimationMgr::Release( void )
{
	size_t iVectorSize = m_vecAnimationController.size();

	for( size_t i = 0; i < iVectorSize; ++i )
		::Safe_Release( m_vecAnimationController[ i ] );

	m_vecAnimationController.erase( m_vecAnimationController.begin(), m_vecAnimationController.end() );
	m_vecAnimationController.swap( vector<CAnimationController*>{} );

	CChameleonGlobalAnimation::DestroyInstance();
	CChameleonIdleAnimation::DestroyInstance();
	CChameleonRunAnimation::DestroyInstance();
	CChameleonJumpAnimation::DestroyInstance();

	delete this;

	return 0;
}

HRESULT CAnimationMgr::Add( CHARACTER_TYPE eCharacterType, STATE eState, const char* pFilePath )
{
	switch( eCharacterType )
	{
	case CHARACTER_CHM :
			switch( eState )
			{
			case STATE_IDLE:
				CChameleonIdleAnimation::GetInstance()->Load( pFilePath );
				break;
			case STATE_RUN:
				CChameleonRunAnimation::GetInstance()->Load( pFilePath );
				break;
			case STATE_JUMP:
				CChameleonJumpAnimation::GetInstance()->Load( pFilePath );
				break;
			default:
				return E_FAIL;
			}
		break;
	case CHARACTER_MON :
		break;
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAnimationMgr::Add( ID3D11Device* pDevice, const char* pFilePath )
{
	m_vecAnimationController.emplace_back( CAnimationController::Create( pDevice, pFilePath ) );

	return S_OK;
}
