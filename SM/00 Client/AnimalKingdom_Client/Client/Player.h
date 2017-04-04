#pragma once

#ifndef Player_h__
#define Player_h__

#include "Physics.h"
#include "GameObject.h"
#include "Protocol.h"

class CAnimator;
class CStateMachine;
class NxController;
class CInputMgr;
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
public:
	CAnimator*	GetAnimator( void ) { return m_pAnimator; }
	CStateMachine* GetFSM( void ) { return m_pStateMachine; }
	DWORD		GetActorCnt( void ) { return m_dwActorCnt; }
	NxController* GetCharacterController( void ) { return m_pCharacterController; }
	XMFLOAT4X4	GetWorld();
	void		Move( NxVec3& vDir, STATE eState );
	void		Sync( NxVec3& vPos, float fRotateY );
public:
	static CPlayer* Create( ID3D11Device* pDevice, NxController* pCharacterController, CHARACTER eType );
protected:
	CStateMachine*			m_pStateMachine;
	CAnimator*				m_pAnimator;
	NxController*			m_pCharacterController;
	XMFLOAT3				m_vRotate;
	DWORD					m_dwActorCnt;
	CInputMgr*				m_pInputMgr;
	float					m_fSpeed;
	NxVec3					m_vDir;
};

#endif // Player_h__