#pragma once
#ifndef InputMgr_h__
#define InputMgr_h__

#include "Singleton.h"

class CInputMgr
	: public CSingleton<CInputMgr>
{
public:
	enum eClick_Type { CLICK_LBUTTON, CLICK_RBUTTON, CLICK_MBUTTON };
	using CLICK_TYPE = eClick_Type;
	enum eMove_Type { MOVE_X, MOVE_Y, MOVE_Z };
	using MOVE_TYPE = eMove_Type;

public:
	BYTE Get_KeyboardState( BYTE KeyFlag );
	BYTE Get_MouseState( CLICK_TYPE KeyFlag );
	long Get_MouseMove( MOVE_TYPE KeyFlag );
public:
	void Check_Input( void );

public:
	HRESULT Initialize( HINSTANCE hInst, HWND hWnd );

private:
	HRESULT Create_Keyboard( HWND hWnd );
	HRESULT Create_Mouse( HWND hWnd );

private:
	LPDIRECTINPUT8			m_pInput;
	LPDIRECTINPUTDEVICE8	m_pKeyboard;
	LPDIRECTINPUTDEVICE8	m_pMouse;

	BYTE					m_byKeyboardState[ 256 ];
	DIMOUSESTATE			m_tMouseState;
public:
	DWORD Release( void );
};

#endif // InputMgr_h__