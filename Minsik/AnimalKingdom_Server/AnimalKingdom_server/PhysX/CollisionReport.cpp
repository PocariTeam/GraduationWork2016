#include "stdafx.h"
#include "CollisionReport.h"
#include "NxPhysics.h"
#include "NxBoxController.h"
#include "NxCapsuleController.h"
#include "NxControllerManager.h"
#include "..\Contents\Player.h"

bool CEntityReport::onEvent( NxU32 nbEntities, NxSweepQueryHit* entities )
{
	for( NxU32 i = 0; i < nbEntities; ++i )
	{
		printf( "[ %d ]��° �浹�� ����: %s ", i, entities[ i ].hitShape->getName() );
	}
	return true;
}

NxControllerAction  CControllerReport::onShapeHit( const NxControllerShapeHit& hit )
{
	//printf(" [NxControllerAction]��Ʈ�ѷ��� �浹����: %s \n", hit.shape->getActor().getName());


	NxActor* actor = hit.controller->getActor();
	NxCollisionGroup group = actor->getGroup();

	if( COL_PLAYER == group )
	{
		STATE eState = ( ( Player* )actor->userData )->getFSM()->GetCurrentState();
		switch( eState )
		{
		case STATE_JUMP:
			( ( Player* )actor->userData )->getFSM()->Change_State( STATE_IDLE );
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
	//UINT roomNum = (UINT)pair.actors[0]->getScene().userData;
	//RoomManager::getInstance().sendDynamicSync(roomNum);

	// �浹������ ������ü �ϳ��� ���� ��ũ 
	//for (int i = 0; i < 2; ++i)
	//{
	//	RoomManager::getInstance().sendDynamicOneSync((UINT)pair.actors[i]->getScene().userData, pair.actors[i]);
	//}
	
	//static int i = 0;
	//printf("Sync count: %d \n",i++);
	printf( " [CCollisionReport] %s �� %s�� �浹!\n ", pair.actors[ 0 ]->getName(), pair.actors[ 1 ]->getName() );
	printf( " [CCollisionReport] %f, %f, %f���� �浹!\n ", pair.actors[ 1 ]->getGlobalPosition().x, pair.actors[ 1 ]->getGlobalPosition().y, pair.actors[ 1 ]->getGlobalPosition().z );
}