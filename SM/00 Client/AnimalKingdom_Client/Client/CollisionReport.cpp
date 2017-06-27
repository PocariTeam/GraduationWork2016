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
#include "Banana.h"
#include "Crown.h"

bool CEntityReport::onEvent( NxU32 nbEntities, NxSweepQueryHit* entities )
{
	for( NxU32 i = 0; i < nbEntities; ++i )
	{
		LONGLONG temp{ ( LONGLONG )entities[ i ].hitShape->getGroup() };
		if( 0 == temp ) return true;
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

	if( ( COL_PLAYER1 | COL_PLAYER2 | COL_PLAYER3 | COL_PLAYER4 ) & group )
	{
		STATE eState = ( ( CPlayer* )actor->userData )->GetFSM()->GetCurrentState();
		switch( eState )
		{
		case STATE_JUMP:
			if( hit.dir.y <= -1.f )
				( ( CPlayer* )actor->userData )->GetAnimator()->Play();
			break;
		default:
			break;
		}
	}

	return NX_ACTION_NONE;
}

NxControllerAction  CControllerReport::onControllerHit( const NxControllersHit& hit )
{
	return NX_ACTION_NONE;
}

void CCollisionReport::onContactNotify( NxContactPair& pair, NxU32 events )
{
	int iBananaIndex{ -1 };

	if( 0 == strcmp( pair.actors[ 1 ]->getName(), "Banana" ) )
		iBananaIndex = 1;
	else if( 0 == strcmp( pair.actors[ 0 ]->getName(), "Banana" ) )
		iBananaIndex = 0;

	if( -1 != iBananaIndex )
	{
		int iNoBananaIndex = ( iBananaIndex == 0 ) ? 1 : 0;

		if( COL_STATIC == COL_GROUP( pair.actors[ iNoBananaIndex ]->getGroup() )
			|| COL_DYNAMIC == COL_GROUP( pair.actors[ iNoBananaIndex ]->getGroup() ) )
		{
			pair.actors[ iBananaIndex ]->setName( "Banana1" );
		}
		else if( !( COL_GROUP( pair.actors[ iNoBananaIndex ]->getGroup() ) & ( ( CBanana* )pair.actors[ iBananaIndex ]->userData )->GetMasterCollisionGroup() ) )
		{
			( ( CPlayer* )pair.actors[ iNoBananaIndex ]->userData )->GetFSM()->Change_State( STATE_BEATEN1 );
			pair.actors[ iBananaIndex ]->setName( "Banana1" );
		}
	}

	int iClothIndex{ -1 };

	if( 0 == strcmp( pair.actors[ 1 ]->getName(), "Cloth" ) )
		iClothIndex = 1;
	else if( 0 == strcmp( pair.actors[ 0 ]->getName(), "Cloth" ) )
		iClothIndex = 0;

	if( -1 != iClothIndex )
	{
		// pair.actors[ iClothIndex ]->getShapes()[ 0 ]->g
	}
}