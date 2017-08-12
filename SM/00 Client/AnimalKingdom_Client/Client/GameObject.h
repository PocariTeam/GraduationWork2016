#pragma once

#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Struct.h"
#include "Function.h"
#include "Base.h"
#include <NxVec3.h>

class NxActor;
class CMesh;
class CTexture;
class CGameObject
	: public CBase
{
protected:
	explicit CGameObject();
	virtual ~CGameObject() = default;
public:
	virtual int		Update( const float& fTimeDelta );
	virtual void	Render( ID3D11DeviceContext* pContext )PURE;
	virtual void	Render_Stencil( ID3D11DeviceContext* pContext ) {}
	virtual DWORD	Release( void );
public:
	void				SetActor( NxActor* pActor ) { m_pActor = pActor; }
	void				SetDir( NxVec3 vDir );
	void				SetOption( int iKey, float fValue );
	virtual void		SetInstanceCnt( UINT iCnt ) { m_iInstanceCnt = iCnt; }
public:
	virtual XMFLOAT4X4* GetWorld();
	NxActor*			GetActor();
	XMFLOAT4			GetOption();
	virtual XMFLOAT4	GetVelocity();
	CTexture*			GetTexture();
protected:
	NxActor*		m_pActor;
	CMesh*			m_pMesh;
	CTexture*		m_pTexture;
	NxVec3			m_vDir;
	XMFLOAT4		m_vOption;
	XMFLOAT4		m_vVelocity;
	XMFLOAT4X4		m_mtxWorld;
	UINT			m_iInstanceCnt;
};

#endif