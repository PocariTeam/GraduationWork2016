#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Tool_Header.h"

class CCamera
{
private:
	HRESULT	Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext );
public:
	DWORD Release( void );
private:
	void	CreateConstantBuffer( ID3D11Device* pDevice );
public:
	void	SetConstantBuffer( ID3D11DeviceContext* pContext );
	void	Update();
	static CCamera*	Create( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext );
public:
	/* 카메라의 포커스 */
	XMFLOAT3	m_vAt{ 0.f, 0.f, 0.f };
	/* 카메라의 포지션 */
	XMFLOAT3	m_vEye{ -50.f, 10.f, 0.f };
	/* 카메라 회전각 x, y, z */
	XMFLOAT3	m_vDegree{ 0.f, 0.f, 0.f };

	ID3D11Buffer*	m_pCbMtxCamera{ nullptr };
	XMFLOAT4X4A		m_mtxView;
	XMFLOAT4X4A		m_mtxProj;
};

#endif	