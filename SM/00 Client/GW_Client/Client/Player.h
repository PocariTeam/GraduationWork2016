#pragma once

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "GameObject.h"

class CPlayer
	: public CGameObject
{
private:
	CPlayer() = default;
	virtual ~CPlayer() = default;
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
private:
	void	KeyCheck( float fTimeDelta );
public:
	static	CPlayer*	Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext
		, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vPos );
private:
	HRESULT	Add_Component( CMesh* pMesh, CTexture* pTexture );
private:
	float			m_fSpeed{ 10.f };
};

#endif