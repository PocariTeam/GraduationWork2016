#pragma once

#ifndef Wallpaper_h__
#define Wallpaper_h__

#include "GameObject.h"

class CWallpaper
	: public CGameObject
{
private:
	explicit CWallpaper();
	virtual ~CWallpaper();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, CMesh* pMesh, CTexture* pTexture );
public:
	static	CWallpaper*	Create( ID3D11Device* pDevice, CMesh* pMesh, CTexture* pTexture = nullptr );
public:
	XMFLOAT4X4*		GetWorld();
	void			Render( ID3D11DeviceContext* pContext );
	DWORD			Release( void );
};

#endif // Wallpaper_h__