#pragma once

#ifndef Monkey_h__
#define Monkey_h__

#include "Player.h"

class NxActor;
class CMonkey
	: public CPlayer
{
public:
	explicit CMonkey();
	virtual ~CMonkey();
private:
	HRESULT	Initialize( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose );
public:
	virtual int		Update( const float& fTimeDelta );
	virtual void	Render( ID3D11DeviceContext* pContext );
	virtual DWORD	Release( void );
	XMFLOAT4X4		GetWorld();
	void			ThrowBanana();
public:
	static CMonkey* Create( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose );
};

#endif // Monkey_h__