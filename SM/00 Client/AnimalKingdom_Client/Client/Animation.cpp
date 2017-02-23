#include "stdafx.h"
#include "Animation.h"
#include <fstream>

void operator >> ( ifstream& pIn, XMFLOAT4X4& mtxAnimation )
{
	for( int i = 0; i < 4; ++i )
		for( int j = 0; j < 4; ++j )
			pIn >> mtxAnimation.m[ i ][ j ];
}

HRESULT CAnimation::Load( const char* pFilePath )
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

void CAnimation::GetAnimationMatrix( XMFLOAT4X4* pOut, float fTimePos )
{
	if( fTimePos < m_dwLength )
		for( DWORD i = 0; i < m_dwJointCnt; ++i )
			pOut[ i ] = m_dpArrFrame[ ( int )fTimePos ][ i ];
	// memcpy_s( pOut, sizeof( XMFLOAT4X4 ) * m_dwJointCnt, m_dpArrFrame[ ( int )fTimePos ], sizeof( XMFLOAT4X4 ) * m_dwJointCnt );
}

DWORD CAnimation::Release( void )
{
	for( DWORD i = 0; i < m_dwLength; ++i )
		::Safe_Delete_Array( m_dpArrFrame[ i ] );
	Safe_Delete_Array( m_dpArrFrame );

	return 0;
}
