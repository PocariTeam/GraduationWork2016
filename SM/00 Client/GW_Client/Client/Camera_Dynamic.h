#pragma once

#ifndef _Camera_Dynamic_H_
#define _Camera_Dynamic_H_

#include "Camera.h"

class CCamera_Dynamic :
	public CCamera
{
private:
	CCamera_Dynamic() = default;
	virtual ~CCamera_Dynamic() = default;
private:
	virtual HRESULT	Initialize( HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext );
public:
	static CCamera_Dynamic* Create( HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
public:
	int   Update( float fTimeDelta );
	DWORD Release();
private:
	void Check_Key( XMVECTOR& vEye, XMVECTOR& vAt, float fTimeDelta );
private:
	HWND			m_hWnd{ nullptr };
	bool			m_bOnClick{ false };
	POINT			m_ptMouse{};
};

#endif