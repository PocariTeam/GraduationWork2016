#pragma once

#ifndef Wallpaper_h__
#define Wallpaper_h__

#include "GameObject.h"

class CMesh;
class CWallpaper
	: public CGameObject
{
private:
	explicit CWallpaper();
	virtual ~CWallpaper();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, CMesh* pMesh );
public:
	static	CWallpaper*	Create( ID3D11Device* pDevice, CMesh* pMesh );
public:
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
private:
	CMesh*		m_pMesh;
};

#endif // Wallpaper_h__