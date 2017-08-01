#pragma once

#ifndef Trail_h__
#define Trail_h__

#include "ParticleObject.h"

class CTrail
	: public CParticleObject
{
private:
	explicit CTrail();
	virtual ~CTrail();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, const XMFLOAT3& vPos );
public:
	static CTrail* Create( ID3D11Device* pDevice, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, const XMFLOAT3& vPos );
public:
	XMFLOAT4X4* GetWorld();
public:
	int		Update( const float& fTimeDelta );
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
};


#endif // Trail_h__