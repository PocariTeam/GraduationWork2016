#include "stdafx.h"
#include "AnimationSet.h"
#include "Function.h"
#include <fstream>

void operator >> ( ifstream& pIn, XMFLOAT4X4& mtxAnimation )
{
	for( int i = 0; i < 4; ++i )
		for( int j = 0; j < 4; ++j )
			pIn >> mtxAnimation.m[ i ][ j ];
}

CAnimationSet::CAnimationSet( void )
	: CBase()
	, m_dpArrFrame( nullptr )
	, m_dwJointCnt( 0 )
	, m_dwLength( 0 )
	, m_fSpeed( 0.f )
	, m_fTimePos( 0.f )
{

}

CAnimationSet::CAnimationSet( const CAnimationSet& Instance )
	: CBase( Instance )
	, m_dpArrFrame( Instance.m_dpArrFrame )
	, m_dwJointCnt( Instance.m_dwJointCnt )
	, m_dwLength( Instance.m_dwLength )
	, m_fSpeed( Instance.m_fSpeed )
	, m_fTimePos( 0.f )
{

}

CAnimationSet* CAnimationSet::Create( const char* pFilePath )
{
	CAnimationSet* pAnimationSet = new CAnimationSet;

	if( FAILED( pAnimationSet->Load( pFilePath ) ) )
	{
		pAnimationSet->Release();
		pAnimationSet = nullptr;
	}

	return pAnimationSet;
}

HRESULT CAnimationSet::Load( const char* pFilePath )
{
	ifstream pIn{ pFilePath };
	char szType{};
	pIn >> szType;

	if( 'a' == szType )
	{
		pIn >> m_dwJointCnt;
		pIn >> m_dwLength;
		pIn >> m_fSpeed;

		m_dpArrFrame = new XMFLOAT4X4*[ m_dwLength ];

		for( DWORD i = 0; i < m_dwLength; ++i )
			m_dpArrFrame[ i ] = new XMFLOAT4X4[ m_dwJointCnt ];

		for( DWORD i = 0; i < m_dwJointCnt; ++i )
			for( DWORD j = 0; j < m_dwLength; ++j )
				pIn >> m_dpArrFrame[ j ][ i ];
	}

	pIn.close();

	return S_OK;
}

void CAnimationSet::GetAnimationMatrix( XMFLOAT4X4* pOut )
{
	if( m_fTimePos < m_dwLength )
		memcpy_s( pOut, sizeof( XMFLOAT4X4 ) * m_dwJointCnt, m_dpArrFrame[ ( int )m_fTimePos ], sizeof( XMFLOAT4X4 ) * m_dwJointCnt );
}

DWORD CAnimationSet::Release( void )
{
	DWORD dwRefCnt = CBase::Release();

	if( 0 == dwRefCnt )
	{
		for( DWORD i = 0; i < m_dwLength; ++i )
			::Safe_Delete_Array( m_dpArrFrame[ i ] );
		Safe_Delete_Array( m_dpArrFrame );
	}
	
	delete this;

	return 0;
}

void CAnimationSet::Update( const float& fTimeDelta )
{
	m_fTimePos += fTimeDelta * m_fSpeed;

	if( m_fTimePos >= ( float )m_dwLength )
		m_fTimePos = 0.f;
}
