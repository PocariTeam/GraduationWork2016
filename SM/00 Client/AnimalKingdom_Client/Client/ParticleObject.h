#pragma once

#ifndef ParticleObject_h__
#define ParticleObject_h__

#include "GameObject.h"

class CParticleObject
	: public CGameObject
{
protected:
	explicit CParticleObject();
	virtual ~CParticleObject();
public:
	virtual XMFLOAT4X4* GetWorld();
	virtual	LPVOID		GetParticleData() { return nullptr; }
	virtual UINT		GetParticleDataCnt() { return 0; }
public:
	virtual int		Update( const float& fTimeDelta );
	virtual void	Render( ID3D11DeviceContext* pContext ) {}
	virtual	void	UpdateParticleValue( const XMFLOAT4X4& mtxWorld ) {}
	virtual	void	SetParticleValue( const XMFLOAT4X4& mtxWorld ) {}
	DWORD	Release( void );
public:
	void	Active( void ) { m_bActive = true; }
	void	Hide( void ) { m_bActive = false; }
protected:
	bool	m_bActive;
};


#endif // ParticleObject_h__