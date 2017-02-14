#include "stdafx.h"
#include "Background.h"

CBackground::CBackground()
	: CMesh()
{
}


CBackground::CBackground( const CBackground& Instance )
	: CMesh( Instance )
{
}

CBackground::~CBackground()
{
}

CMesh* CBackground::Clone( void )
{
	return new CBackground( *this );
}

HRESULT CBackground::CreateBuffer( void )
{
	m_dwVtxCnt = 6;

	return S_OK;
}

CBackground* CBackground::Create( void )
{
	CBackground*	pBackground = new CBackground;

	if( FAILED( pBackground->CreateBuffer() ) )
	{
		pBackground->Release();
		pBackground = nullptr;
	}

	return pBackground;
}

DWORD CBackground::Release( void )
{
	DWORD dwRefCnt = CMesh::Release();
	delete this;

	return dwRefCnt;
}