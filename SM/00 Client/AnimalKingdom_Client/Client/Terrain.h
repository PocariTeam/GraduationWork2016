#pragma once

#ifndef Terrain_h__
#define Terrain_h__

#include "GameObject.h"

class CMesh;
class CTexture;
class CTerrain
	: public CGameObject
{
private:
	explicit CTerrain();
	virtual ~CTerrain();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, CMesh* pMesh, CTexture* pTexture );
	void		CreateConstantBuffer( ID3D11Device* pDevice );
public:
	static CTerrain* Create( ID3D11Device* pDevice, CMesh* pMesh, CTexture* pTexture );
public:
	void	Render( ID3D11DeviceContext* pContext );
	void	SetConstantBuffer( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
private:
	CMesh*		m_pMesh;
	CTexture*	m_pTexture;
};

#endif // Terrain_h__
