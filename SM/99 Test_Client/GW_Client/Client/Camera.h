#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "GameObject.h"
#include "Struct.h"

class CGraphicDev;
class CCamera
	: public CGameObject
{
protected:
	CCamera() = default;
	virtual ~CCamera() = default;
protected:
	virtual HRESULT	Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext );
public:
	virtual int Update( float fTimeDelta ) { return 0; }
	virtual DWORD Release( void );
public:
	virtual void	CreateConstantBuffer( ID3D11Device* pDevice );
	virtual void	SetConstantBuffer( CGraphicDev* pGraphicDev );
	virtual void	SetConstantBuffer( ID3D11DeviceContext* pContext );
protected:
	/* 카메라의 포커스 */
	XMFLOAT3	m_vAt{ 0.f, 0.f, 0.f };
	/* 카메라의 포지션 */
	XMFLOAT3	m_vEye{ 0.f, 0.f, 0.f };
	/* 카메라 회전각 x, y, z */
	XMFLOAT3	m_vDegree{ 0.f, 0.f, 0.f };

	ID3D11Buffer*	m_pCbMtxCamera;
	XMFLOAT4X4A		m_mtxView;
	XMFLOAT4X4A		m_mtxProj;
	float			m_fSpeed{ 20.f };
};

#endif	