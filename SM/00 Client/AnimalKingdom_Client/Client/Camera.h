#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Base.h"

class CCamera
	: public CBase
{
protected:
	explicit CCamera();
	virtual ~CCamera() = default;
protected:
	virtual HRESULT	Initialize( ID3D11Device* pDevice );
public:
	virtual int Update( const float& fTimeDelta )PURE;
	virtual void Render( ID3D11DeviceContext* pContext );
	virtual DWORD Release( void );
protected:
	void		CreateConstantBuffer( ID3D11Device* pDevice );
	void		SetConstantBuffer( ID3D11DeviceContext* pContext );
	void		CalculateViewMatrix( XMFLOAT4X4* pOut, const XMFLOAT3& vEye, const XMFLOAT3& vAt );
protected:
	ID3D11Buffer*	m_pConstantBufferCamera;

	XMFLOAT3		m_vEye;
	XMFLOAT3		m_vAt;
	XMFLOAT4X4		m_mtxView;
	XMFLOAT4X4		m_mtxProj;
	XMFLOAT4X4		m_mtxOrtho;
	float			m_fSpeed;
};

#endif	