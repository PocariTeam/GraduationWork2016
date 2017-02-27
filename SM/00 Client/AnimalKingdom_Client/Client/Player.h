#pragma once

#ifndef Player_h__
#define Player_h__

#include "GameObject.h"
#include "AnimationMgr.h"

class CAnimationController;
class NxActor;
class CPlayer
	: public CGameObject
{
protected:
	explicit CPlayer();
	virtual ~CPlayer();
protected:
	virtual HRESULT Initialize( ID3D11Device* pDevice, NxActor* pActor );
public:
	void	Check_Key( const float& fTimeDelta );
public:
	virtual int Update( const float& fTimeDelta );
	virtual void Render( ID3D11DeviceContext* pContext );
	virtual DWORD Release( void );
public:
	static CPlayer* Create( ID3D11Device* pDevice, NxActor* pActor, CAnimationMgr::CHARACTER_TYPE eType );
protected:
	CAnimationController*	m_pAnimationController;
};

#endif // Player_h__