#include "stdafx.h"
#include "AnimationMgr.h"
#include "Animator.h"
#include <fstream>

CAnimationMgr::~CAnimationMgr()
{
	this->Release();
}

CAnimator* CAnimationMgr::Clone( const CHARACTER& eType )
{
	CAnimator*	pController = Find( eType );
	if( nullptr == pController ) return nullptr;

	return pController->Clone();
}

CAnimator* CAnimationMgr::Find( const CHARACTER& eType )
{
	if( eType < CHARACTER_MAX )
		return m_vecAnimator[ eType ];

	return nullptr;
}

HRESULT CAnimationMgr::Load( const char* pFilePath )
{
	m_vecAnimator.reserve( ( size_t )CHARACTER_MAX );

	ifstream pIn{ pFilePath };

	if (pIn.is_open() == false)
	{
		SLog(L"! '%S' open fail! ", pFilePath);
		return S_FALSE;
	}

	int iCharacterCnt{}, iCharacterType{}, iStateCnt{}, iState{};
	char szPath[ MAX_PATH ]{""};
	
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
				Add( szPath );
			else
				Add( ( CHARACTER )iCharacterType, ( STATE )iState, szPath );
		}
	}

	pIn.close();

	return S_OK;
}

DWORD CAnimationMgr::Release( void )
{
	size_t iVectorSize = m_vecAnimator.size();

	for( size_t i = 0; i < iVectorSize; ++i )
	{
		if( m_vecAnimator[ i ] )
		{
			m_vecAnimator[ i ]->Release();
			m_vecAnimator[ i ] = nullptr;
		}
	}
	m_vecAnimator.erase( m_vecAnimator.begin(), m_vecAnimator.end() );
	m_vecAnimator.swap( vector<CAnimator*>{} );

	return 0;
}

HRESULT CAnimationMgr::Add( CHARACTER eCharacterType, STATE eState, const char* pFilePath )
{
	m_vecAnimator[ eCharacterType ]->Add( eState, pFilePath );

	return S_OK;
}

HRESULT CAnimationMgr::Add( const char* pFilePath )
{
	m_vecAnimator.emplace_back( CAnimator::Create( pFilePath ) );

	return S_OK;
}