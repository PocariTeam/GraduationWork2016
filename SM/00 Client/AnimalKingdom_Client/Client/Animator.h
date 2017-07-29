#pragma once

#ifndef Animator_h__
#define Animator_h__

#include "Enum.h"
#include "Base.h"
#include <NxActor.h>
#include <NxMat34.h>

class CGameObject;
class CAnimationSet;
class CAnimator
	: public CBase
{
private:
	explicit CAnimator( void );
	explicit CAnimator( const CAnimator& Instance );
public:
	void Render( ID3D11DeviceContext* pContext );
	void Update( CGameObject* pOwner, const float& fTimeDelta );
	CAnimator* Clone( void );
	static CAnimator* Create( ID3D11Device* pDevice, const char* pFilePath );
	DWORD Release( void );
private:
	HRESULT	Initialize( ID3D11Device* pDevice, const char* pFilePath );
	HRESULT Load( const char* pFilePath );
	void	ConnectActorShape( CGameObject* pOwner );
public:
	NxMat34 GetCurrentAnimationMatrix( CGameObject* pOwner, const char* pKey );
	NxActor* GetActor( CGameObject* pOwner, const char* pKey );
	HRESULT Add( STATE eState, const char* pFilePath );
	void	Change_Animation( STATE eState );
	bool	GetCurrentAnimationFinished();
	float	GetPerFinish();
	void	Pause();
	void	Play();
	bool	isPause() { return m_bPause; }
private:
	HRESULT CreateConstantBuffer( ID3D11Device* pDevice );
private:
	CAnimationSet*			m_pCurrentAnimationSet;
	CAnimationSet*			m_pPreviousAnimationSet;

	map<int, int>			m_mapEvent;
	vector<CAnimationSet*>	m_vecAnimationSet;
	string*					m_pArrJointName;
	DWORD					m_dwJointCnt;
	bool					m_bPause;
	ID3D11Buffer*			m_pConstantBufferAnimation;
};

#endif // AnimationController_h__
