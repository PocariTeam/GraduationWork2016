#pragma once

#ifndef Normal_UI_h__
#define Normal_UI_h__

#include "UserInterface.h"

class CNormal_UI
	: public CUserInterface
{
private:
	explicit CNormal_UI();
	virtual ~CNormal_UI();
private:
	HRESULT		Initialize( CTexture* pTexture, const XMFLOAT4& vOffset );
public:
	static CNormal_UI*	Create( CTexture* pTexture, const XMFLOAT4& vOffset );
public:
	void Render( ID3D11DeviceContext* pContext );
	int Update( const float& fTimeDelta );
	DWORD Release( void );
public:
	void Hide() { m_bHide = true; }
	void Show() { m_bHide = false; }
private:
	bool m_bHide;
};

#endif // Normal_UI_h__