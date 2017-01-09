#pragma once
#include "NxPhysics.h"

enum CollGroup
{
	STATIC,
	DYNAMIC,
	MY_CHARACTER,
	OTHER_CHARACTER
};

#define COLLIDABLE_MASK	(1<<STATIC) | (1<<DYNAMIC)

class UserEntityReport : public NxUserEntityReport<NxSweepQueryHit> {
public:
	virtual bool onEvent(NxU32 nbEntities, NxSweepQueryHit* entities);
};


class ControllerHitReport : public NxUserControllerHitReport
{
public:
	virtual NxControllerAction  onShapeHit(const NxControllerShapeHit& hit);
	virtual NxControllerAction  onControllerHit(const NxControllersHit& hit);

};