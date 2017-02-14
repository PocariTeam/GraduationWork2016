#pragma once

#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Struct.h"
#include "Base.h"

class NxActor;
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
	virtual void	CreateConstantBuffer( ID3D11Device* pDevice ){};
	virtual void	SetConstantBuffer( ID3D11DeviceContext* pContext ) {};
	void			SetActor( NxActor* pActor ) { m_pActor = pActor; }
protected:
	ID3D11Buffer*	m_pConstantBufferWorld;
	ID3D11Buffer*	m_pConstantBufferMaterial;
	NxActor*		m_pActor;
};

#endif