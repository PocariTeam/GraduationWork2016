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
	virtual HRESULT Initialize( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose, int iID );
public:
	void			Check_Key( const float& fTimeDelta );
	void			Jump( const float& fTimeDelta, float fAnimatePercent );
public:
	void			Move( const float& fTimeDelta );
	void			Sync( NxVec3& vPos, float hp, float fRotateY, STATE state , FLOAT canUseSkill);
	virtual void	Attack( STATE eState );
	virtual void	UseSkill();
	virtual void	ThrowBanana( void );
	virtual void	SweapOn( void );
	virtual void	SweapOff( void );
public:
	virtual int		Update( const float& fTimeDelta );
	virtual void	Render( ID3D11DeviceContext* pContext );
	void			Render_Dummy( ID3D11DeviceContext* pContext );
	void			Render_Stencil( ID3D11DeviceContext* pContext );
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
	bool			InCave() { return m_bInCave; }
	float*			GetHP() { return &m_fHp; }
	STATE			GetCurrentState();
	float			GetRotateY() { return m_vRotate.y; }
	float*			GetCanUseSkill() { return &m_fCanUseSkill; }
	void			SetSkillOn(BOOL s) { m_bSkillOn = s; }
	BOOL			GetSkillOn() { return m_bSkillOn; }
	int				GetID() { return m_iID; }
public:
	void			Change_State( STATE eState );
public:
	static CPlayer* Create( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPoseArray, CHARACTER eType, int iID );
protected:
	int						m_iID;
	float					m_fHp;
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
	bool					m_bInCave;

	BOOL					m_bSkillOn;
	float					m_fDefenceTime;
	FLOAT					m_fCanUseSkill;
};

#endif // Player_h__