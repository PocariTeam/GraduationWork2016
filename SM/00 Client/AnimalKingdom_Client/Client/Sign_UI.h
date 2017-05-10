#pragma once

#ifndef Sign_UI_h__
#define Sign_UI_h__

#include "UserInterface.h"

class CSign_UI
	: public CUserInterface
{
private:
	explicit CSign_UI();
	virtual ~CSign_UI();
private:
	HRESULT		Initialize( CTexture* pTexture, XMFLOAT4X4* pDest, UINT iID );
public:
	static CSign_UI*	Create( CTexture* pTexture, XMFLOAT4X4* pDest, UINT iID );
public:
	void Render( ID3D11DeviceContext* pContext );
	int Update( const float& fTimeDelta );
	DWORD Release( void );
public:
	void Hide() { m_bHide = true; }
	void Show() { m_bHide = false; }
	void SetNumber( UINT iNum );
private:
	bool		m_bHide;
	XMFLOAT4X4*	m_pDestWorld;
	float		m_fOffset;
	UINT		m_iValue;
};

#endif // Sign_UI_h__
