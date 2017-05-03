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
#include <NxShape.h>
#include "Enum.h"

bool CEntityReport::onEvent( NxU32 nbEntities, NxSweepQueryHit* entities )
{
	for( NxU32 i = 0; i < nbEntities; ++i )
	{
		LONGLONG temp{ ( LONGLONG )entities[ i ].hitShape->getGroup() };
		if( !( ( LONGLONG )entities->userData & temp ) )
		{
			// printf( "내 그룹 : %lld\n", ( LONGLONG )entities->userData );
			// printf( "얘 그룹 : %lld\n", temp = entities[ i ].hitShape->getGroup() );
			// printf( "E [ %d ]번째 충돌한 도형: %s \n", i, entities[ i ].hitShape->getName() );
			( ( CPlayer* )entities[ i ].hitShape->getActor().userData )->GetFSM()->Change_State( STATE_BEATEN1 );
			return true;
		}
	}

	return true;
}

NxControllerAction  CControllerReport::onShapeHit( const NxControllerShapeHit& hit )
{
	//printf("컨트롤러와 충돌액터: %s \n", hit.shape->getActor().getName());

	NxActor* actor = hit.controller->getActor();
	NxCollisionGroup group = actor->getGroup();

	if( hit.dir.y > -1.f ) return NX_ACTION_NONE;

	if( ( COL_PLAYER1 | COL_PLAYER2 | COL_PLAYER3 | COL_PLAYER4 ) & group )
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