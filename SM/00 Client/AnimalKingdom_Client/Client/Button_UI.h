#pragma once

#ifndef Button_UI_h__
#define Button_UI_h__

#include "UserInterface.h"

class CWindow_UI;
class CButton_UI
	: public CUserInterface
{
public:
	enum eButton_state { BTN_NORMAL, BTN_ACTIVE, BTN_PUSH, BTN_PLAYING, BTN_END };
	using BTN_STATE = eButton_state;
private:
	explicit CButton_UI();
	virtual ~CButton_UI();
private:
	HRESULT		Initialize( CWindow_UI* pOwner, CTexture* pTexture, const XMFLOAT4& vOffset );
public:
	static CButton_UI*	Create( CWindow_UI* pOwner, CTexture* pTexture, const XMFLOAT4& vOffset );
public:
	void Render( ID3D11DeviceContext* pContext );
	int Update( const float& fTimeDelta );
	DWORD Release( void );
public:
	bool isCollide( POINT& ptMouse, bool bClick );
private:
	CWindow_UI*		m_pOwner;
	XMFLOAT4		m_vOffset;
	BTN_STATE		m_eState;
};

#endif // Button_UI_h__