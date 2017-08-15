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
	HRESULT Initialize( ID3D11Device* pDevice, XMFLOAT4X4* pWorldTranspose, XMFLOAT3& vOffset );
public:
	static CThirdCamera*	Create( ID3D11Device* pDevice, XMFLOAT4X4* pWorldTranspose, XMFLOAT3& vOffset );
public:
	void   SetDestWorldTranspose( XMFLOAT4X4* pWorldTranspose ) { m_pDestWorldTranspose = pWorldTranspose; }
	void   WinnerEvent();
	void   SetOffset( XMFLOAT3& vOffset ) { m_vDestOffset = vOffset; }
private:
	XMFLOAT3	m_vDestOffset;
	XMFLOAT4X4*	m_pDestWorldTranspose;
	XMFLOAT3	m_vOffset;
	XMFLOAT3	m_vCurrent;
	bool		m_bWinnerEvent;
public:
	int Update( const float& fTimeDelta );
	DWORD Release( void );
};

#endif // ThirdCamera_h__