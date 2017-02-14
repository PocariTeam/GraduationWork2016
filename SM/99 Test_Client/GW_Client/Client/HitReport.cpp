#include "stdafx.h"
#include "NxBoxController.h"
#include "NxCapsuleController.h"
#include "NxControllerManager.h"
#include "HitReport.h"

// sweep �浹(�÷��̾�-�÷��̾�)
EntityReport gUserEntityReport;
// ��Ʈ�ѷ� �浹
ControllerHitReport gControllerHitReport;
// ���� �浹 (���� �浹�ϴ� ��ü�鳢���� ����)
ContactReport gContactReport;

bool EntityReport::onEvent(NxU32 nbEntities, NxSweepQueryHit* entities) {

	for (NxU32 i = 0; i < nbEntities; ++i)
	{
		printf("[%d]��° �浹�� ����: %s \n", i, entities[i].hitShape->getName());
	}
	return true;
}

NxControllerAction  ControllerHitReport::onShapeHit(const NxControllerShapeHit& hit)
{
	//printf("��Ʈ�ѷ��� �浹����: %s \n", hit.shape->getActor().getName());

	if (1 && hit.shape)
	{
		NxCollisionGroup group = hit.shape->getGroup();
		if (group == DYNAMIC)
		{
			NxActor& actor = hit.shape->getActor();
			// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
			// useless stress on the solver. It would be possible to enable/disable vertical pushes on
			// particular objects, if the gameplay requires it.
			if (hit.dir.y == 0.0f)
			{
				NxF32 coeff = actor.getMass() * hit.length * 10.0f;
				actor.addForceAtLocalPos(hit.dir*coeff, NxVec3(0, 0, 0), NX_IMPULSE);
				//						actor.addForceAtPos(hit.dir*coeff, hit.controller->getPosition(), NX_IMPULSE);
				//						actor.addForceAtPos(hit.dir*coeff, hit.worldPos, NX_IMPULSE);
			}
		}
	}

	return NX_ACTION_NONE;
}

NxControllerAction  ControllerHitReport::onControllerHit(const NxControllersHit& hit)
{
	return NX_ACTION_NONE;
}


void ContactReport::onContactNotify(NxContactPair& pair, NxU32 events)
{
	printf("%s �� %s�� �浹! \n", pair.actors[0]->getName(), pair.actors[1]->getName());
}