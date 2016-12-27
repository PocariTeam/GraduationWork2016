#pragma once

#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Transform.h"
#include "Struct.h"
#include "Base.h"

class CGraphicDev;
class CComponent;
class CMesh;
class CTexture;
class CGameObject
	: public CBase
{
protected:
	CGameObject() = default;
	virtual ~CGameObject() = default;
public:
	virtual int		Update( float fTimeDelta );
	virtual void	Render( CGraphicDev* pGraphicDev ) {}
	virtual void	Render( ID3D11DeviceContext* pContext, DWORD dwInstanceCnt = 1 ) {}
	virtual void	Render_Reflect( ID3D11DeviceContext* pContext, DWORD dwInstanceCnt = 1 ) {}
	virtual DWORD	Release( void );
	virtual void	SetConstantBuffer( CGraphicDev* pGraphicDev ) {}
	virtual void	SetConstantBuffer( ID3D11DeviceContext* pContext ) {}

public:
	CMesh	*		Get_Mesh() { return m_pMeshCom; }
	CTexture*		Get_Texture() { return m_pTexture; }
	CTransform*		Get_Transform() { return m_pTransCom; }
	XMFLOAT3*		Get_Pos() { if( m_pTransCom ) return &m_pTransCom->m_vPos; return nullptr; }
	XMFLOAT3*		Get_Scale() { if( m_pTransCom ) return &m_pTransCom->m_vScale; return nullptr; }
	XMFLOAT4X4A*	Get_World() { if( m_pTransCom ) return &m_pTransCom->m_mtxWorld; return nullptr; }
protected:
	map< const char*, CComponent*> m_mapComponent;

	CTransform*				m_pTransCom{ nullptr };
	CMesh*					m_pMeshCom{ nullptr };
	CTexture*				m_pTexture{ nullptr };
	MATERIAL				m_tMaterial;
	ID3D11Buffer*			m_pCBmtxWorld{ nullptr };
	ID3D11Buffer*			m_pCBMaterial{ nullptr };
};

#endif