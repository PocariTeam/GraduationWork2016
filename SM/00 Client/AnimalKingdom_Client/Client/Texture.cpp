#include "stdafx.h"
#include "Texture.h"


CTexture::CTexture()
	: CResources()
{
}


CTexture::CTexture( const CTexture& Instance )
	: CResources( Instance )
{

}

CTexture::~CTexture()
{
}

DWORD CTexture::Release()
{
	return CResources::Release();
}
