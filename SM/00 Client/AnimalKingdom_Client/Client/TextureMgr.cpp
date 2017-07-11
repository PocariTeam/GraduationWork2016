#include "stdafx.h"
#include "TextureMgr.h"
#include <fstream>
#include "Texture.h"
#include "Functor.h"
#include "MultiTexture.h"
#include "SingleTexture.h"

CTextureMgr* CSingleton<CTextureMgr>::m_pInstance;

DWORD CTextureMgr::Release( void )
{
	for_each( m_mapTexture.begin(), m_mapTexture.end(), ReleasePair() );
	delete this;
	return 0;
}

CTexture* CTextureMgr::Clone( const char* pKey )
{
	CTexture*		pTexture = Find( pKey );
	if( nullptr == pTexture )	return nullptr;
	return pTexture->Clone();
}

CTexture* CTextureMgr::Find( const char* pKey )
{
	TEXTURECONTAINER::iterator	iter_Find = m_mapTexture.find( pKey );
	if( iter_Find == m_mapTexture.end() )	return nullptr;
	return iter_Find->second;
}

HRESULT CTextureMgr::Add( ID3D11Device* pDevice, const char* pTextureKey, const char* pPath, int iTextureCnt /*= 1 */ )
{
	CTexture*		pTexture = Find( pTextureKey );

	if( nullptr != pTexture )
	{
		MessageBox( nullptr, "Already Texture Exists", nullptr, MB_OK );
		return E_FAIL;
	}

	switch( iTextureCnt )
	{
	case 1 :
		pTexture = CSingleTexture::Create( pDevice, pPath );
		break;
	default:
		pTexture = CMultiTexture::Create( pDevice, pPath, iTextureCnt );
		break;
	}

	if( nullptr == pTexture ) {
		MessageBox( nullptr, "Texture Creature Failed", nullptr, MB_OK );
		return E_FAIL;
	}
	
	m_mapTexture.insert( make_pair( pTextureKey, pTexture ) );
	return S_OK;
}

HRESULT CTextureMgr::Load( ID3D11Device* pDevice, const char* pFilePath )
{
	HRESULT		hr{};
	ifstream	pIn{ pFilePath };
	int			iAllCnt{}, iSoleCnt{};
	char		szKey[ MAX_PATH ]{ "" }, szPath[ MAX_PATH ]{ "" };

	pIn >> iAllCnt;

	for( int i = 0; i < iAllCnt; ++i )
	{
		pIn >> szKey;
		pIn >> iSoleCnt;
		pIn >> szPath;

		Add( pDevice, szKey, szPath, iSoleCnt );
	}

	pIn.close();
	return S_OK;
}
