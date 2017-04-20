#include "stdafx.h"
#include "AnimationMgr.h"
#include "Animator.h"
#include <fstream>

CAnimationMgr*	CSingleton<CAnimationMgr>::m_pInstance;

CAnimator* CAnimationMgr::Clone( const CHARACTER& eType )
{
	CAnimator*	pController = Find( eType );
	if( nullptr == pController ) return nullptr;

	return pController->Clone();
}

CAnimator* CAnimationMgr::Find( const CHARACTER& eType )
{
	if( eType < CHARACTER_END )
		return m_vecAnimator[ eType ];

	return nullptr;
}

HRESULT CAnimationMgr::Load( ID3D11Device* pDevice, const char* pFilePath )
{
	m_vecAnimator.reserve( ( size_t )CHARACTER_END );

	ifstream pIn{ pFilePath };
	int iCharacterCnt{}, iCharacterType{}, iStateCnt{}, iState{};
	char szPath[ MAX_PATH ]{ "" };

	pIn >> iCharacterCnt;

	for( int i = 0; i < iCharacterCnt; ++i )
	{
		pIn >> iCharacterType;
		pIn >> iStateCnt;

		for( int j = 0; j < iStateCnt; ++j )
		{
			pIn >> iState;
			pIn >> szPath;

			if( -1 == iState )
				Add( pDevice, szPath );
			else
				Add( ( CHARACTER )iCharacterType, ( STATE )iState, szPath );
		}
	}

	return S_OK;
}

DWORD CAnimationMgr::Release( void )
{
	size_t iVectorSize = m_vecAnimator.size();

	for( size_t i = 0; i < iVectorSize; ++i )
		::Safe_Release( m_vecAnimator[ i ] );

	m_vecAnimator.erase( m_vecAnimator.begin(), m_vecAnimator.end() );
	m_vecAnimator.swap( vector<CAnimator*>{} );

	delete this;

	return 0;
}

HRESULT CAnimationMgr::Add( CHARACTER eCharacterType, STATE eState, const char* pFilePath )
{
	m_vecAnimator[ eCharacterType ]->Add( eState, pFilePath );

	return S_OK;
}

HRESULT CAnimationMgr::Add( ID3D11Device* pDevice, const char* pFilePath )
{
	m_vecAnimator.emplace_back( CAnimator::Create( pDevice, pFilePath ) );

	return S_OK;
}