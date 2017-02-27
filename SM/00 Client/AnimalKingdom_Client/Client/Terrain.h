#pragma once

#ifndef Terrain_h__
#define Terrain_h__

#include "GameObject.h"

class CTerrain
	: public CGameObject
{
private:
	explicit CTerrain();
	virtual ~CTerrain();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture );
public:
	static CTerrain* Create( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture );
public:
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
};

#endif // Terrain_h__
