#pragma once

#ifndef Banana_h__
#define Banana_h__

#include "GameObject.h"
#include "Enum.h"

class CBanana
	: public CGameObject
{
private:
	explicit CBanana();
	virtual ~CBanana();
private:
	HRESULT		Initialize( NxActor* pActor, COL_GROUP eMaster );
public:
	static CBanana* Create( NxActor* pActor, COL_GROUP eMaster );
public:
	XMFLOAT4X4 GetWorld();
public:
	int			Update( const float& fTimeDelta );
	void		Throw( NxVec3& vPos, NxVec3& vDir, COL_GROUP eMaster );
	void		Frozen( void );
	void		Render( ID3D11DeviceContext* pContext );
	DWORD		Release( void );
	COL_GROUP	GetMasterCollisionGroup() const;
private:
	XMFLOAT3	m_vScale;
	COL_GROUP	m_eMasterGroup;
};

#endif // Banana_h__
