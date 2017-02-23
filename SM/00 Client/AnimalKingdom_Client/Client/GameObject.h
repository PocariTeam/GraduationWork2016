#pragma once

#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Struct.h"
#include "Base.h"

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
	virtual DWORD	Release( void );
public:
	void			SetActor( NxActor* pActor ) { m_pActor = pActor; }
	XMFLOAT4X4*		GetWorld() { return &m_mtxWorld; }
protected:
	NxActor*		m_pActor;
	XMFLOAT4X4		m_mtxWorld;
	CMesh*			m_pMesh;
	CTexture*		m_pTexture;
};

#endif