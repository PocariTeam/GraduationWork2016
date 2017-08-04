#pragma once

#ifndef Player_h__
#define Player_h__

#include "Physics.h"
#include "GameObject.h"

class CAnimator;
class CStateMachine;
class NxController;
class CSection;
class CInputMgr;
class CPlayer
	: public CGameObject
{
protected:
	explicit CPlayer();
	virtual ~CPlayer();
protected:
	virtual HRESULT Initialize( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose );
public:
	void			Check_Key( const float& fTimeDelta );
	void			Jump( const float& fTimeDelta, float fAnimatePercent );
public:
	void			Move( const float& fTimeDelta );
	void			Sync( NxVec3& vPos, int hp, float fRotateY, STATE state);
	virtual void	Attack( STATE eState );
	virtual void	UseSkill();
	virtual void	ThrowBanana( void );
	virtual void	SweapOn( void );
	virtual void	SweapOff( void );
public:
	virtual int		Update( const float& fTimeDelta );
	virtual void	Render( ID3D11DeviceContext* pContext );
	virtual DWORD	Release( void );
public:
	void			MinimizeController( void );
	CHARACTER		GetCharacterType( void ) { return m_eCharactor; }
	void			SetSection( CSection** dpSections, UINT iSectionCnt );
	CAnimator*		GetAnimator( void ) { return m_pAnimator; }
	CStateMachine*	GetFSM( void ) { return m_pStateMachine; }
	DWORD			GetActorCnt( void ) { return m_dwActorCnt; }
	NxController*	GetCharacterController( void ) { return m_pCharacterController; }
	NxMat34*		GetActorsOriginPose( void ) { return m_pActorsOriginPose; }
	virtual XMFLOAT4X4*	GetWorld();
	void			ResetOverlapped();
	bool			GetAlpha() { return m_bAlpha; }
	int*			GetHP() { return &m_iHp; }
	STATE			GetCurrentState();
	float			GetRotateY() { return m_vRotate.y; }
	void			SetSkillOn(bool s) { m_bSkillOn = s; }
public:
	void			Change_State( STATE eState );
public:
	static CPlayer* Create( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPoseArray, CHARACTER eType );
protected:
	int						m_iHp;
	CHARACTER				m_eCharactor;
	NxVec3					m_vOverlapped;
	CStateMachine*			m_pStateMachine;
	CAnimator*				m_pAnimator;
	NxController*			m_pCharacterController;
	NxMat34*				m_pActorsOriginPose;
	XMFLOAT3				m_vRotate;
	DWORD					m_dwActorCnt;
	CInputMgr*				m_pInputMgr;
	bool					m_bSweap;
	CSection**				m_dpSections;
	UINT					m_iSectionCnt;
	bool					m_bAlpha;

	bool					m_bSkillOn;
	float					m_fDefenceTime;
};

#endif // Player_h__