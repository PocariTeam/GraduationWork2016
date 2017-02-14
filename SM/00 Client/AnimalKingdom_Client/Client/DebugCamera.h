#pragma once

#ifndef DebugCamera_h__
#define DebugCamera_h__

#include "Camera.h"

class CDebugCamera
	: public CCamera
{
private:
	explicit CDebugCamera();
	virtual ~CDebugCamera();
private:
	HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice );
	void	Check_Key( XMVECTOR& vEye, XMVECTOR& vAt, const float& fTimeDelta );
public:
	static CDebugCamera*	Create( HWND hWnd, ID3D11Device* pDevice );
private:
	HWND	m_hWnd;
	bool	m_bClicked;
	POINT	m_ptMouse;
public:
	int Update( const float& fTimeDelta );
	DWORD Release( void );
};

#endif // DebugCamera_h__