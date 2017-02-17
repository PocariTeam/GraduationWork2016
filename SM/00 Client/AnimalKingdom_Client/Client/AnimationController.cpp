#include "stdafx.h"
#include "AnimationController.h"
#include "Animation.h"

template<class CHAR_TYPE>
CAnimationController<CHAR_TYPE>::~CAnimationController()
{
}

template<class CHAR_TYPE>
void CAnimationController<CHAR_TYPE>::Update( const float& fTimeDelta )
{
	if( m_pPreviousAnimation ) m_pPreviousAnimation->Execute( m_pOwner, fTimeDelta );
	if( m_pCurrentAnimation ) m_pCurrentAnimation->Execute( m_pOwner, fTimeDelta );
}

template<class CHAR_TYPE>
void CAnimationController<CHAR_TYPE>::Change_Animation( CAnimation<CHAR_TYPE>* pAnimation )
{
	if( nullptr == pAnimation )
		MessageBox( nullptr, "Change_Animation Parameter is nullptr", nullptr, MB_OK );

	m_pPreviousAnimation = m_pCurrentAnimation;
	m_pCurrentAnimation->Exit( m_pOwner );
	m_pCurrentAnimation = pAnimation;
	m_pCurrentAnimation->Enter( m_pOwner );
}

template<class CHAR_TYPE>
void CAnimationController<CHAR_TYPE>::SetPreviousAnimation( CAnimation<CHAR_TYPE>* pAnimation )
{
	m_pPreviousAnimation = pAnimation;
}

template<class CHAR_TYPE>
void CAnimationController<CHAR_TYPE>::SetGlobalAnimation( CAnimation<CHAR_TYPE>* pAnimation )
{
	m_pGlobalAnimation = pAnimation;
}

template<class CHAR_TYPE>
void CAnimationController<CHAR_TYPE>::SetCurrentAnimation( CAnimation<CHAR_TYPE>* pAnimation )
{
	m_pPreviousAnimation = pAnimation;
}


template<class CHAR_TYPE>
void CAnimationController<CHAR_TYPE>::RevertToPreviousAnimation( void )
{
	Change_Animation( m_pPreviousAnimation );
}

template<class CHAR_TYPE>
CAnimationController<CHAR_TYPE>::CAnimationController( CHAR_TYPE* pOwner )
	: CBase()
	, m_pOwner( pOwner )
	, m_pCurrentAnimation( nullptr )
	, m_pPreviousAnimation( nullptr )
	, m_pGlobalAnimation( nullptr )
{

}

template<class CHAR_TYPE>
CAnimationController<CHAR_TYPE>* CAnimationController<CHAR_TYPE>::Clone( void )
{
	return new CAnimationController( *this );
}
