#pragma once

#ifndef Sign_UI_h__
#define Sign_UI_h__

#include "UserInterface.h"

class CShader;
class CPlayer;
class CSign_UI
	: public CUserInterface
{
private:
	explicit CSign_UI();
	virtual ~CSign_UI();
private:
	HRESULT		Initialize( CTexture* pTexture, CPlayer* pOwner, XMFLOAT4X4* pDest, UINT iID );
public:
	static CSign_UI*	Create( CTexture* pTexture, CPlayer* pOwner, XMFLOAT4X4* pDest, UINT iID );
public:
	void Render( ID3D11DeviceContext* pContext );
	int Update( const float& fTimeDelta );
	DWORD Release( void );
public:
	void Hide() { m_bHide = true; }
	void Show() { m_bHide = false; }
	void SetNumber( UINT iNum );
	void SetPosition( XMFLOAT2& vPos ) {}
private:
	CPlayer*	m_pOwner;
	CShader*	m_pShader;
	bool		m_bHide;
	XMFLOAT4X4*	m_pDestWorld;
	float		m_fOffset;
	UINT		m_iValue;
};

#endif // Sign_UI_h__
