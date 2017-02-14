#include "stdafx.h"
#include "Texture.h"

CTexture::CTexture()
	: CBase()
{
}

CTexture::CTexture( const CTexture & Instance )
	: CBase( Instance )
{
}


CTexture::~CTexture()
{
}

DWORD CTexture::Release( void )
{
	DWORD dwRefCnt = CBase::Release();

	return dwRefCnt;
}
