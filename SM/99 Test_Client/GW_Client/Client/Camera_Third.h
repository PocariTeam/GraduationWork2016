#pragma once

#ifndef _Camera_Third_H_
#define _Camera_Third_H_

#include "Camera.h"

class CTransform;
class CCamera_Third :
	public CCamera
{
private:
	CCamera_Third() = default;
	virtual ~CCamera_Third() = default;
private:
	virtual HRESULT	Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CTransform* pTarget );
public:
	static CCamera_Third* Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTarget );
public:
	int   Update( float fTimeDelta );
	DWORD Release();
private:
	void Check_Key( float fTimeDelta );
private:
	CTransform*		m_pTarget{ nullptr };
	float			m_fAngle{ 0.f };
	XMFLOAT3		m_vOffset{ 0.f, 0.f, -60.f };
};

#endif