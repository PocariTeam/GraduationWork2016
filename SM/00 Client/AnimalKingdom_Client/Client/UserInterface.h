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
	virtual void	Render( ID3D11DeviceContext* pContext );
	virtual int		Update( const float& fTimedelta );
	virtual DWORD	Release( void );
public:
	virtual void	SetTexture( CTexture* pTexture );
	virtual void	SetSize( XMFLOAT2& vSize );
	virtual void	SetPosition( XMFLOAT2& vPos ) { m_mtxWorld._11 = vPos.x; m_mtxWorld._21 = vPos.y; }
protected:
	XMFLOAT4	m_vPosSize_Dest;
	XMFLOAT4	m_vPosSize_Src;
	float		m_fLerpTime;
};


#endif // UserInterface_h__
