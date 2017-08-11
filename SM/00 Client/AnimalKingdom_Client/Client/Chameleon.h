#pragma once

#ifndef Chameleon_h__
#define Chameleon_h__

#include "Player.h"

class NxActor;
class CEffectObject;
class CChameleon
	: public CPlayer
{
public:
	explicit CChameleon();
	virtual ~CChameleon();
private:
	HRESULT	Initialize( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose, CHARACTER eType, int iID );
public:
	virtual int Update( const float& fTimeDelta );
	virtual void Render( ID3D11DeviceContext* pContext );
	virtual DWORD Release( void );
	virtual void Attack( STATE eState );
	XMFLOAT4X4*	GetWorld();
	virtual void	SweapOff( void );
public:
	static CChameleon* Create( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose, CHARACTER eType, int iID );
private:
	CEffectObject*	m_pTrail[ 2 ];
};

#endif // Chameleon_h__