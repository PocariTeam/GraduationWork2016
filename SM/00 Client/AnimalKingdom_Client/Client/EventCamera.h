#pragma once

#ifndef EventCamera_h__
#define EventCamera_h__

#include "Camera.h"

class NxActor;
class CEventCamera
	: public CCamera
{
private:
	explicit CEventCamera();
	virtual ~CEventCamera();
private:
	HRESULT Initialize( ID3D11Device* pDevice, XMFLOAT3& vAt, XMFLOAT3& vDest, XMFLOAT3& vCurrent );
public:
	static CEventCamera*	Create( ID3D11Device* pDevice, XMFLOAT3& vAt, XMFLOAT3& vDest, XMFLOAT3& vCurrent );
private:
	XMFLOAT3	m_vDest;
public:
	int Update( const float& fTimeDelta );
	DWORD Release( void );
};

#endif // EventCamera_h__