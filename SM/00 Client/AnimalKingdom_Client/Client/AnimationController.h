#pragma once

#ifndef AnimationController_h__
#define AnimationController_h__

#include "Enum.h"
#include "Base.h"

class CGameObject;
class CAnimation;
class CAnimationController
	: public CBase
{
private:
	explicit CAnimationController( void );
	explicit CAnimationController( const CAnimationController& Instance, CGameObject* pOwner );
public:
	void Update( const float& fTimeDelta );
	void Render( ID3D11DeviceContext* pContext );
	CAnimationController* Clone( CGameObject* pOwner );
	static CAnimationController* Create( ID3D11Device* pDevice, const char* pFilePath );
	DWORD Release( void );
private:
	HRESULT	Initialize( ID3D11Device* pDevice, const char* pFilePath );
	HRESULT Load( const char* pFilePath );
	HRESULT CreateConstantBuffer( ID3D11Device* pDevice );
private:
	CGameObject*	m_pOwner;

	CAnimation*	m_pCurrentAnimation;
	CAnimation*	m_pPreviousAnimation;
	CAnimation*	m_pGlobalAnimation;

	string*					m_pArrJointName;
	DWORD					m_dwJointCnt;
	float					m_fTimePos;
	ID3D11Buffer*			m_pConstantBufferAnimation;
public:
	void	SetCurrentAnimation( CAnimation* pAnimation );
	void	SetGlobalAnimation( CAnimation* pAnimation );
	void	SetPreviousAnimation( CAnimation* pAnimation );
	void	Change_Animation( CAnimation* pAnimation );
	void	RevertToPreviousAnimation( void );
};

#endif // AnimationController_h__
