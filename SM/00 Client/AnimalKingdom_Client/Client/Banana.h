#pragma once

#ifndef Banana_h__
#define Banana_h__

#include "GameObject.h"

class CBanana
	: public CGameObject
{
private:
	explicit CBanana();
	virtual ~CBanana();
private:
	HRESULT		Initialize( NxActor* pActor, NxVec3& vDir );
public:
	static CBanana* Create( NxActor* pActor, NxVec3& vDir );
public:
	XMFLOAT4X4 GetWorld();
public:
	int		Update( const float& fTimeDelta );
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
private:
	XMFLOAT3	m_vScale;
};

#endif // Banana_h__
