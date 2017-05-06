#pragma once

#ifndef Banana_h__
#define Banana_h__

#include "PhysX\CollisionReport.h"

class CBanana
{
private:
	explicit CBanana();
	virtual ~CBanana();
private:
	HRESULT		Initialize( NxActor* pActor, NxVec3& vDir, COL_GROUP eMaster );
public:
	static CBanana* Create( NxActor* pActor, NxVec3& vDir, COL_GROUP eMaster );
public:
	XMFLOAT4X4 GetWorld();
	COL_GROUP	GetMasterCollisionGroup();
public:
	DWORD	Release( void );
private:
	NxActor*	m_pActor;
	COL_GROUP	m_eMasterGroup;
};

#endif // Banana_h__