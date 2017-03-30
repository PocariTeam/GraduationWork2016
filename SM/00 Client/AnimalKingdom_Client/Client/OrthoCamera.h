#pragma once

#ifndef OrthoCamera_h__
#define OrthoCamera_h__

#include "Camera.h"

class COrthoCamera
	: public CCamera
{
private:
	explicit COrthoCamera();
	virtual ~COrthoCamera();
private:
	HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice );
public:
	static COrthoCamera*	Create( HWND hWnd, ID3D11Device* pDevice );
private:
	HWND	m_hWnd;
public:
	int Update( const float& fTimeDelta );
	DWORD Release( void );
};

#endif // OrthoCamera_h__