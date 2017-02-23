#pragma once

#ifndef Animation_h__
#define Animation_h__

#include "Enum.h"
#include "Singleton.h"


class CGameObject;
class CAnimation
{
public:
	HRESULT Load( const char* pFilePath );
	void	GetAnimationMatrix( XMFLOAT4X4* pOut, float fTimePos );
	virtual void Enter( CGameObject* pOwner, const float& fTimeDelta, float& fTimePos )PURE;
	virtual void Execute( CGameObject* pOwner, const float& fTimeDelta, float& fTimePos )PURE;
	virtual void Exit( CGameObject* pOwner, const float& fTimeDelta, float& fTimePos )PURE;
	virtual DWORD Release( void );

protected:
	XMFLOAT4X4**	m_dpArrFrame{ nullptr };
	DWORD			m_dwJointCnt{ 0 };
	DWORD			m_dwLength{ 0 };
	float			m_fSpeed{ 0 };
};

#endif // Animation_h__