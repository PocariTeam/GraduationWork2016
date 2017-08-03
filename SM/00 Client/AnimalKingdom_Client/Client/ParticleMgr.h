#pragma once

#ifndef ParticleMgr_h__
#define ParticleMgr_h__

#include "Singleton.h"
#include "Struct.h"

class CShader;
class CParticleObject;
class CParticleMgr
	: public CSingleton<CParticleMgr>
{
public:
	enum eParticle_Type	{ PARTICLE_TRAIL, /*PARTICLE_FIRE, */PARTICLE_END };
	using PARTICLE_TYPE = eParticle_Type;
public:
	HRESULT	Initialize( ID3D11Device* pDevice );
	DWORD	Release( void );
	CParticleObject* Add( PARTICLE_TYPE eType, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, XMFLOAT3& vVelocity = XMFLOAT3{ 0.f, 0.f, 0.f } );
public:
	void	Update( const float& fTimeDelta );
	void	Render( ID3D11DeviceContext* pContext );
private:
	ID3D11Device*	m_pDevice;
	CShader*		m_pParticleShaderArr[ PARTICLE_END ]{};
};

#endif // ParticleMgr_h__