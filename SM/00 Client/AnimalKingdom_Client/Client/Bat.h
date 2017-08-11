#pragma once

#ifndef Bat_h__
#define Bat_h__

#include "Player.h"

class NxActor;
class CEffectObject;
class CBat
	: public CPlayer
{
public:
	explicit CBat();
	virtual ~CBat();
private:
	HRESULT	Initialize( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose, CHARACTER eType, int iID );
public:
	virtual int Update( const float& fTimeDelta );
	virtual void Render( ID3D11DeviceContext* pContext );
	virtual DWORD Release( void );
	virtual void Attack( STATE eState );
	XMFLOAT4X4*	GetWorld();
public:
	static CBat* Create( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose, CHARACTER eType, int iID );
};

#endif // Bat_h__