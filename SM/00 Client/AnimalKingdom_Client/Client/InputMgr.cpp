#include "stdafx.h"
#include "InputMgr.h"
#include "Function.h"

CInputMgr* CSingleton<CInputMgr>::m_pInstance;

bool CInputMgr::Get_KeyboardState( BYTE KeyFlag )
{
	return ( m_byKeyboardState[ KeyFlag ] & 0x80 ) ? true : false;
}

bool CInputMgr::Get_MouseState( CLICK_TYPE KeyFlag )
{
	return ( m_tMouseState.rgbButtons[ KeyFlag ] & 0x80 ) ? true : false;
}

long CInputMgr::Get_MouseMove( MOVE_TYPE KeyFlag )
{
	return *( ( ( long* )&m_tMouseState ) + KeyFlag );
}

void CInputMgr::Check_Input( void )
{
	m_pKeyboard->GetDeviceState( 256, m_byKeyboardState );
	m_pMouse->GetDeviceState( sizeof( DIMOUSESTATE ), &m_tMouseState );
}

HRESULT CInputMgr::Initialize( HINSTANCE hInst, HWND hWnd )
{
	if( FAILED( DirectInput8Create( hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, ( void** )&m_pInput, nullptr ) ) )
	{
		MessageBox( nullptr, "XInput Create Failed", nullptr, MB_OK );
		return E_FAIL;
	}

	if( FAILED( Create_Keyboard( hWnd ) ) )
	{
		MessageBox( nullptr, "XInput - Keyboard Create Failed", nullptr, MB_OK );
		return E_FAIL;
	}

	if( FAILED( Create_Mouse( hWnd ) ) )
	{
		MessageBox( nullptr, "XInput - Mouse Create Failed", nullptr, MB_OK );
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CInputMgr::Create_Keyboard( HWND hWnd )
{
	if( FAILED( m_pInput->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, nullptr ) ) )
		return E_FAIL;

	m_pKeyboard->SetCooperativeLevel( hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY );

	if( FAILED( m_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
		return E_FAIL;

	if( FAILED( m_pKeyboard->Acquire() ) )
		return E_FAIL;

	return S_OK;
}

HRESULT CInputMgr::Create_Mouse( HWND hWnd )
{
	if( FAILED( m_pInput->CreateDevice( GUID_SysMouse, &m_pMouse, NULL ) ) )
		return E_FAIL;

	if( FAILED( m_pMouse->SetCooperativeLevel( hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE ) ) )
		return E_FAIL;

	if( FAILED( m_pMouse->SetDataFormat( &c_dfDIMouse ) ) )
		return E_FAIL;

	if( FAILED( m_pMouse->Acquire() ) )
		return E_FAIL;

	return S_OK;
}

DWORD CInputMgr::Release( void )
{
	Safe_Release( m_pMouse );
	Safe_Release( m_pKeyboard );
	Safe_Release( m_pInput );

	delete this;

	return 0;
}
