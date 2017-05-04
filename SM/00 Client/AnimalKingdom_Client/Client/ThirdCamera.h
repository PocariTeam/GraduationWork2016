#pragma once

#ifndef ThirdCamera_h__
#define ThirdCamera_h__

#include "Camera.h"

class NxActor;
class CThirdCamera
	: public CCamera
{
private:
	explicit CThirdCamera();
	virtual ~CThirdCamera();
private:
	HRESULT Initialize( ID3D11Device* pDevice, NxActor* pActor, XMFLOAT3& vOffset );
public:
	static CThirdCamera*	Create( ID3D11Device* pDevice, NxActor* pActor, XMFLOAT3& vOffset );
private:
	NxActor*	m_pDestActor;
	XMFLOAT3	m_vOffset;
public:
	int Update( const float& fTimeDelta );
	DWORD Release( void );
};

#endif // ThirdCamera_h__