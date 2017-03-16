#pragma once

#ifndef UserInterface_h__
#define UserInterface_h__

#include "GameObject.h"

class CUserInterface
	: public CGameObject
{
private:
	explicit CUserInterface();
	virtual ~CUserInterface();
private:
	HRESULT		Initialize( CTexture* pTexture, const XMFLOAT4& vPosSize, const float& fPriority );
public:
	static CUserInterface*	Create( CTexture* pTexture, const XMFLOAT4& vPosSize, const float& fPriority );
public:
	XMFLOAT4X4	GetWorld( void );
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
private:
	XMFLOAT4X4	m_mtxWolrd;
	// 11 : posX, 12 : posY, 13 : sizeX, 14 : sizeY, 21 : Priority
};


#endif // UserInterface_h__
