#pragma once

#ifndef Player_h__
#define Player_h__

#include "GameObject.h"
#include "Enum.h"

class CAnimator;
class CStateMachine;
class NxController;
class CPlayer
	: public CGameObject
{
protected:
	explicit CPlayer();
	virtual ~CPlayer();
protected:
	virtual HRESULT Initialize( ID3D11Device* pDevice, NxController* pCharacterController );
public:
	void	Check_Key( const float& fTimeDelta );
public:
	virtual int Update( const float& fTimeDelta );
	virtual void Render( ID3D11DeviceContext* pContext );
	virtual DWORD Release( void );
	CAnimator*	GetAnimator( void ) { return m_pAnimator; }
	CStateMachine* GetFSM( void ) { return m_pStateMachine; }
	DWORD		GetActorCnt( void ) { return m_dwActorCnt; }
	XMFLOAT4X4	GetWorld();
	NxController* GetCharacterController( void ) { return m_pCharacterController; }
public:
	static CPlayer* Create( ID3D11Device* pDevice, NxController* pCharacterController, CHARACTER eType );
protected:
	CStateMachine*			m_pStateMachine;
	CAnimator*				m_pAnimator;
	NxController*			m_pCharacterController;
	XMFLOAT3				m_vRotate;
	DWORD					m_dwActorCnt;
};

#endif // Player_h__