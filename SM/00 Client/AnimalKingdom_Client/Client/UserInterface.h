#pragma once

#ifndef UserInterface_h__
#define UserInterface_h__

#include "GameObject.h"

class CUserInterface
	: public CGameObject
{
protected:
	explicit CUserInterface();
	virtual ~CUserInterface();
public:
	virtual XMFLOAT4X4	GetWorld( void );
	virtual void	Render( ID3D11DeviceContext* pContext );
	virtual int		Update( const float& fTimedelta );
	virtual DWORD	Release( void );
public:
	virtual void	SetTexture( CTexture* pTexture );
protected:
	XMFLOAT4X4	m_mtxPosSize;
	XMFLOAT4	m_vPosSize_Dest;
	XMFLOAT4	m_vPosSize_Src;
	float		m_fLerpTime;
};


#endif // UserInterface_h__
