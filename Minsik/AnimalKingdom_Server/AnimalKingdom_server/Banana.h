#pragma once

#ifndef Banana_h__
#define Banana_h__

#include "PhysX\CollisionReport.h"

#define FROZEN_DELAY_TICK		3000


class CBanana
{
private:
	explicit CBanana();
	virtual ~CBanana();
private:
	HRESULT		Initialize( NxActor* pActor, COL_GROUP eMaster, UINT iSceneNum );
public:
	static CBanana* Create( NxActor* pActor, COL_GROUP eMaster, UINT iSceneNum );
public:
	XMFLOAT4X4	GetWorld();
	COL_GROUP	GetMasterCollisionGroup() const;
	void		Throw( NxVec3& vPos, NxVec3& vDir, COL_GROUP eMaster );
	void		Frozen( void );
public:
	DWORD	Release( void );
private:
	NxActor*	m_pActor;
	COL_GROUP	m_eMasterGroup;
	UINT		m_iSceneNum;
};

#endif // Banana_h__