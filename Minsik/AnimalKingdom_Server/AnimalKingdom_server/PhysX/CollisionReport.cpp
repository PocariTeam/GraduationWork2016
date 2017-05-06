#include "stdafx.h"
#include "CollisionReport.h"
#include "NxPhysics.h"
#include "NxBoxController.h"
#include "NxCapsuleController.h"
#include "NxControllerManager.h"
#include "..\Contents\Player.h"
#include "..\Banana.h"

bool CEntityReport::onEvent( NxU32 nbEntities, NxSweepQueryHit* entities )
{
	for( NxU32 i = 0; i < nbEntities; ++i )
	{
		LONGLONG temp{ ( LONGLONG )entities[ i ].hitShape->getGroup() };
		if( 0 == temp ) return true;
		if( !( ( LONGLONG )entities->userData & temp ) )
		{
			// printf( "내 그룹 : %lld\n", ( LONGLONG )entities->userData );
			// printf( "얘 그룹 : %lld\n", temp );
			// printf( "E [ %d ]번째 충돌한 도형: %s \n", i, entities[ i ].hitShape->getName() );
			( ( Player* )entities[ i ].hitShape->getActor().userData )->getFSM()->Change_State( STATE_BEATEN1 );
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
		STATE eState = ( ( Player* )actor->userData )->getFSM()->GetCurrentState();
		switch( eState )
		{
		case STATE_JUMP:
			if( hit.dir.y <= -1.f )
				( ( Player* )actor->userData )->getAnimator()->Play();
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
	int iBananaIndex{ -1 };

	if( 0 == strcmp( pair.actors[ 1 ]->getName(), "Banana" ) )
		iBananaIndex = 1;
	else if( 0 == strcmp( pair.actors[ 0 ]->getName(), "Banana" ) )
		iBananaIndex = 0;

	if( -1 != iBananaIndex )
	{
		int iNoBananaIndex = ( iBananaIndex == 0 ) ? 1 : 0;

		// printf( "C %s 와 %s가 충돌! \n", pair.actors[ 0 ]->getName(), pair.actors[ 1 ]->getName() );
		if( ( COL_GROUP( pair.actors[ iBananaIndex ]->getGroup() ) & ( COL_STATIC | COL_DYNAMIC ) ) )
		{
			pair.actors[ iBananaIndex ]->setName( "Banana1" );
		}

		else if( !( COL_GROUP( pair.actors[ iBananaIndex ]->getGroup() ) & ( ( CBanana* )pair.actors[ 1 ]->userData )->GetMasterCollisionGroup() ) )
		{
			( ( Player* )pair.actors[ iNoBananaIndex ]->userData )->getFSM()->Change_State( STATE_BEATEN1 );
			pair.actors[ iBananaIndex ]->setName( "Banana1" );
		}
	}

	//UINT roomNum = (UINT)pair.actors[0]->getScene().userData;
	//RoomManager::getInstance().sendDynamicSync(roomNum);

	// 충돌감지된 동적객체 하나에 대한 싱크 
	//for (int i = 0; i < 2; ++i)
	//{
	//	RoomManager::getInstance().sendDynamicOneSync((UINT)pair.actors[i]->getScene().userData, pair.actors[i]);
	//}
	
	//static int i = 0;
	//printf("Sync count: %d \n",i++);
	//printf( " [CCollisionReport] %s 와 %s가 충돌!\n ", pair.actors[ 0 ]->getName(), pair.actors[ 1 ]->getName() );
	//printf( " [CCollisionReport] %f, %f, %f에서 충돌!\n ", pair.actors[ 1 ]->getGlobalPosition().x, pair.actors[ 1 ]->getGlobalPosition().y, pair.actors[ 1 ]->getGlobalPosition().z );
}