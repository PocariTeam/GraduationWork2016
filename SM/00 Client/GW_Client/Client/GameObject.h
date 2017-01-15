#pragma once

#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Transform.h"
#include "Struct.h"
#include "Base.h"

class NxActor;
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
	void			SetActor( NxActor* pActor ) { m_pActor = pActor;  }
public:
	CMesh	*		Get_Mesh() { return m_pMeshCom; }
	CTexture*		Get_Texture() { return m_pTexture; }
protected:
	map< const char*, CComponent*> m_mapComponent;

	CMesh*					m_pMeshCom{ nullptr };
	CTexture*				m_pTexture{ nullptr };
	MATERIAL				m_tMaterial;
	ID3D11Buffer*			m_pCBmtxWorld{ nullptr };
	ID3D11Buffer*			m_pCBMaterial{ nullptr };
	NxActor*				m_pActor{ nullptr };
};

#endif