#pragma once

#ifndef Trail_h__
#define Trail_h__

#include "GameObject.h"

class CTrail
	: public CGameObject
{
private:
	explicit CTrail();
	virtual ~CTrail();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, XMFLOAT3& vScale );
public:
	static CTrail* Create( ID3D11Device* pDevice, XMFLOAT3& vScale );
public:
	XMFLOAT4X4* GetWorld();
public:
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
private:
	XMFLOAT3	m_vScale;
};


#endif // Trail_h__