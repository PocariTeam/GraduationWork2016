#pragma once

#ifndef TextureMgr_h__
#define TextureMgr_h__

#include "Singleton.h"

class CTexture;
class CTextureMgr
	: public CSingleton<CTextureMgr>
{
public:
	DWORD	Release( void );
public:
	CTexture*	Clone( const char* pKey );
	CTexture*	Find( const char* pKey );
public:
	HRESULT Add( ID3D11Device* pDevice, const char* pTextureKey, const char* pPath, int iTextureCnt = 1 );
	HRESULT Load( ID3D11Device* pDevice, const char* pFilePath );
private:
	using TEXTURECONTAINER = map<const string, CTexture*>;
	TEXTURECONTAINER	m_mapTexture;
};

#endif // TextureMgr_h__