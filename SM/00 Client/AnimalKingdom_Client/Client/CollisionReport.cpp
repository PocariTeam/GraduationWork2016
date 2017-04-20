#include "stdafx.h"
#include "CollisionReport.h"
#include "NxPhysics.h"
#include "NxBoxController.h"
#include "NxCapsuleController.h"
#include "NxControllerManager.h"
#include "Player.h"
#include <NxActor.h>
#include "StateMachine.h"
#include "Animator.h"

bool CEntityReport::onEvent( NxU32 nbEntities, NxSweepQueryHit* entities )
{
	for( NxU32 i = 0; i < nbEntities; ++i )
	{
		printf( "E [ %d ]번째 충돌한 도형: %s \n", i, entities[ i ].hitShape->getName() );
	}

	return true;
}

NxControllerAction  CControllerReport::onShapeHit( const NxControllerShapeHit& hit )
{
	//printf("컨트롤러와 충돌액터: %s \n", hit.shape->getActor().getName());

	NxActor* actor = hit.controller->getActor();
	NxCollisionGroup group = actor->getGroup();

	if( hit.dir.y > -1.f ) return NX_ACTION_NONE;

	if( COL_MINE == group )
	{
		STATE eState = ( ( CPlayer* )actor->userData )->GetFSM()->GetCurrentState();
		switch( eState )
		{
		case STATE_JUMP:
			( ( CPlayer* )actor->userData )->GetAnimator()->Play();
			break;
		default:
			break;
		}

		/*if( 0.f == hit.dir.y )
		{
		NxF32 coeff = actor.getMass() * hit.length * 10.0f;
		actor.addForceAtLocalPos( hit.dir*coeff, NxVec3( 0, 0, 0 ), NX_IMPULSE );
		}*/
	}

	return NX_ACTION_NONE;
}

NxControllerAction  CControllerReport::onControllerHit( const NxControllersHit& hit )
{
	return NX_ACTION_NONE;
}

void CCollisionReport::onContactNotify( NxContactPair& pair, NxU32 events )
{
	printf( "C %s 와 %s가 충돌! \n", pair.actors[ 0 ]->getName(), pair.actors[ 1 ]->getName() );
}