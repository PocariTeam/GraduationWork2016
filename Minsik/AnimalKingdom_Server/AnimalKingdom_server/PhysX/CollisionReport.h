#pragma once

#ifndef CollisionReport_h__
#define CollisionReport_h__

enum CollGroup { COL_STATIC = 1, COL_DYNAMIC = 2, COL_PLAYER1 = 4, COL_PLAYER2 = 8, COL_PLAYER3 = 16, COL_PLAYER4 = 32, COL_END };
using COL_GROUP = CollGroup;

#include <NxUserEntityReport.h>
#include <NxScene.h>
#include <NxController.h>
#include <NxUserContactReport.h>

#define COLLIDABLE_MASK	( 1 << COL_STATIC ) | ( 1 << COL_DYNAMIC )

class CEntityReport : public NxUserEntityReport<NxSweepQueryHit>
{
public:
	virtual bool onEvent( NxU32 nbEntities, NxSweepQueryHit* entities );
};

class CControllerReport : public NxUserControllerHitReport
{
public:
	virtual NxControllerAction  onShapeHit( const NxControllerShapeHit& hit );
	virtual NxControllerAction  onControllerHit( const NxControllersHit& hit );
};

class CCollisionReport : public NxUserContactReport
{
public:
	virtual void onContactNotify( NxContactPair& pair, NxU32 events );
};

#endif // CollisionReport_h__