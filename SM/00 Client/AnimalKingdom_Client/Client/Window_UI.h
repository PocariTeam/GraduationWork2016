#pragma once

#ifndef Window_UI_h__
#define Window_UI_h__

#include "UserInterface.h"

class CWindow_UI
	: public CUserInterface
{
public:
	enum eWnd_type{ WND_LOBBY, WND_ROOM, WND_END };
	using WND_TYPE = eWnd_type;
private:
	explicit CWindow_UI();
	virtual ~CWindow_UI();
private:
	HRESULT		Initialize( WND_TYPE eWndType );
public:
	static CWindow_UI*	Create( WND_TYPE eWndType );
public:
	void Render( ID3D11DeviceContext* pContext );
	int	Update( const float& fTimeDelta );
	DWORD Release( void );
public:
	XMFLOAT4	GetPosSize( void );
};

#endif // Window_UI_h__