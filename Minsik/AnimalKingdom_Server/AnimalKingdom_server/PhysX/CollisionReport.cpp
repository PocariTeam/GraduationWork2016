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
		if (0 == temp || temp & COL_DYNAMIC) return true;
		Player* pAttacker = ( Player* )entities [ i ].userData;
		LONGLONG Attacker_Group = ( LONGLONG )( pAttacker->getActors()[0]->getGroup() );
		if( !( Attacker_Group & temp ) )
		{
			// printf( "�� �׷� : %lld\n", ( LONGLONG )entities->userData );
			// printf( "�� �׷� : %lld\n", temp );
			// printf( "E [ %d ]��° �浹�� ����: %s \n", i, entities[ i ].hitShape->getName() );
			
			Player* player = (Player*)entities[i].hitShape->getActor().userData;
			if( ! player->checkBlocking( pAttacker->getRotateY() ) )
				( ( Player* )entities[i].hitShape->getActor().userData)->proceedBeaten( pAttacker->getDamage() );

			return true;
		}
	}

	return true;
}

NxControllerAction  CControllerReport::onShapeHit( const NxControllerShapeHit& hit )
{
	//printf("��Ʈ�ѷ��� �浹����: %s \n", hit.shape->getActor().getName());

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

		if (COL_STATIC == COL_GROUP(pair.actors[iNoBananaIndex]->getGroup())
			|| COL_DYNAMIC == COL_GROUP(pair.actors[iNoBananaIndex]->getGroup()))
		{
			return;
		}
		else if( !( COL_GROUP( pair.actors[ iNoBananaIndex ]->getGroup() ) & ( ( CBanana* )pair.actors[ iBananaIndex ]->userData )->GetMasterCollisionGroup() ) )
		{
			if( !( ( Player* )pair.actors[ iNoBananaIndex ]->userData )->checkBlocking( pair.actors[ iBananaIndex ]->getLinearVelocity() ) )
			{
				// �ӵ�üũ
				NxVec3 velocity = pair.actors[iBananaIndex]->getLinearVelocity();
				const float minimunVelocity = 50.0f;
				if (velocity.magnitude() > minimunVelocity)
				{
					int iDamage = ((CBanana*)pair.actors[iBananaIndex]->userData)->m_pPlayer->getDamage();
					((Player*)pair.actors[iNoBananaIndex]->userData)->proceedBeaten(iDamage);

				}
				
			}
			pair.actors[iBananaIndex]->setLinearVelocity(NxVec3(0.0f, 0.0f, 0.0f));
			( ( CBanana* )pair.actors[ iBananaIndex ]->userData )->Frozen();
		}
	}

	// �հ� �׽�Ʈ
	int iCrownIndex{ -1 };
	if (0 == strcmp(pair.actors[1]->getName(), "Crown"))	iCrownIndex = 1;
	else if (0 == strcmp(pair.actors[0]->getName(), "Crown"))	iCrownIndex = 0;

	if( -1 != iCrownIndex )
	{
		int iNoCrownIndex = ( iCrownIndex == 0 ) ? 1 : 0;
		if( COL_STATIC != COL_GROUP( pair.actors[ iNoCrownIndex ]->getGroup() )
			&& COL_DYNAMIC != COL_GROUP( pair.actors[ iNoCrownIndex ]->getGroup() ) )
		{
			UINT roomNum = (UINT)pair.actors[iCrownIndex]->getScene().userData;
			Player* crownOwner = (Player*)pair.actors[iNoCrownIndex]->userData;
			if (crownOwner->getHp() > 0 &&RoomManager::getInstance().hasWinner(roomNum) == false)
			{
				RoomManager::getInstance().sendGetCrown(roomNum, crownOwner);
				crownOwner->powerUp();
				pair.actors[iCrownIndex]->raiseBodyFlag(NX_BF_KINEMATIC);
				pair.actors[iCrownIndex]->setGlobalPosition(NxVec3(0.0, 1000.0f, 1000.0f));
			}
		}
	}

	//UINT roomNum = (UINT)pair.actors[0]->getScene().userData;
	//RoomManager::getInstance().sendDynamicSync(roomNum);

	// �浹������ ������ü �ϳ��� ���� ��ũ 
	//for (int i = 0; i < 2; ++i)
	//{
	//	RoomManager::getInstance().sendDynamicOneSync((UINT)pair.actors[i]->getScene().userData, pair.actors[i]);
	//}
	
	//static int i = 0;
	//printf("Sync count: %d \n",i++);
	//printf( " [CCollisionReport] %s �� %s�� �浹!\n ", pair.actors[ 0 ]->getName(), pair.actors[ 1 ]->getName() );
	//printf( " [CCollisionReport] %f, %f, %f���� �浹!\n ", pair.actors[ 1 ]->getGlobalPosition().x, pair.actors[ 1 ]->getGlobalPosition().y, pair.actors[ 1 ]->getGlobalPosition().z );
}