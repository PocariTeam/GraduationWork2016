#pragma once

#ifndef ParticleObject_h__
#define ParticleObject_h__

#include "GameObject.h"

class CParticleObject
	: public CGameObject
{
private:
	explicit CParticleObject();
	virtual ~CParticleObject();
private:
	HRESULT		Initialize( ID3D11Device* pDevice );
public:
	static CParticleObject* Create( ID3D11Device* pDevice );
public:
	virtual XMFLOAT4X4* GetWorld();
public:
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
};


#endif // ParticleObject_h__