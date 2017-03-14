#pragma once

#ifndef CollisionReport_h__
#define CollisionReport_h__

enum CollGroup
{
	COL_STATIC,
	COL_DYNAMIC,
	COL_PLAYER
};

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