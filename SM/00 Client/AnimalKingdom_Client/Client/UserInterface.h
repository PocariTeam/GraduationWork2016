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
	// 11 : posX, 12 : posY, 13 : sizeX, 14 : sizeY, 21 : Priority
};


#endif // UserInterface_h__
