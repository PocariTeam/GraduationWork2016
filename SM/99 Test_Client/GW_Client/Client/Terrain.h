#pragma once

#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include "GameObject.h"

class CTerrain
	: public CGameObject
{
private:
	CTerrain() = default;
	virtual ~CTerrain() = default;
private:
	HRESULT	Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vPos );
	void	CreateConstantBuffer( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	void	SetConstantBuffer( ID3D11DeviceContext* pContext );
public:
	int		Update( float fTimeDelta );
	void	Render( CGraphicDev* pGraphicDev );
	void	Render( ID3D11DeviceContext* pContext, DWORD dwInstanceCnt );
	DWORD	Release( void );
public:
	static	CTerrain*	Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext
		, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vPos );
private:
	HRESULT	Add_Component( CMesh* pMesh, CTexture* pTexture );
};

#endif