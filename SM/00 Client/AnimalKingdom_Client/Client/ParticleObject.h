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
public:
	virtual int		Update( const float& fTimeDelta );
	DWORD	Release( void );
};


#endif // ParticleObject_h__