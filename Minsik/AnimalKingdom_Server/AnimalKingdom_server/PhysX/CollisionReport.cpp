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
		if (COL_STATIC == temp || temp & COL_DYNAMIC) return true;
		Player* pAttacker = ( Player* )entities [ i ].userData;
		LONGLONG Attacker_Group = ( LONGLONG )( pAttacker->getActors()[0]->getGroup() );
		if( !( Attacker_Group & temp ) )
		{
			// printf( "내 그룹 : %lld\n", ( LONGLONG )entities->userData );
			// printf( "얘 그룹 : %lld\n", temp );
			// printf( "E [ %d ]번째 충돌한 도형: %s \n", i, entities[ i ].hitShape->getName() );
			
			Player* player = (Player*)entities[i].hitShape->getActor().userData;
			if( ! player->checkBlocking( pAttacker->getRotateY() ) )
				( ( Player* )entities[i].hitShape->getActor().userData)->proceedBeaten( pAttacker->getDamage() );

			return true;
		}
	}

	return true;
}

NxControllerAction  CControllerReport::onShapeHit(const NxControllerShapeHit& hit)
{
	//printf("컨트롤러와 충돌액터: %s \n", hit.shape->getActor().getName());

	NxActor* actor = hit.controller->getActor();
	NxActor& hitActor = hit.shape->getActor();
	NxCollisionGroup group = actor->getGroup();

	// 점프체크
	if ((COL_PLAYER1 | COL_PLAYER2 | COL_PLAYER3 | COL_PLAYER4) & group)
	{
		STATE eState = ((Player*)actor->userData)->getFSM()->GetCurrentState();
		switch (eState)
		{
		case STATE_JUMP:
			if (hit.dir.y <= -1.f)
				((Player*)actor->userData)->getAnimator()->Play();
			break;
		default:
			break;
		}
	}

	// 왕관 체크
	if (0 == strcmp(hitActor.getName(), "Crown"))
	{
		UINT roomNum = (UINT)(actor->getScene().userData);
		Player* crownOwner = (Player*)actor->userData;
		if (crownOwner->getHp() > 0 && RoomManager::getInstance().hasWinner(roomNum) == false)
		{
			RoomManager::getInstance().sendGetCrown(roomNum, crownOwner);
			crownOwner->powerUp();
			hitActor.raiseBodyFlag(NX_BF_KINEMATIC);
			hitActor.setGlobalPosition(NxVec3(0.0, 1000.0f, 1000.0f));
		}
	}

	// 동적객체 밀기
	if (hitActor.getGroup() == COL_DYNAMIC)
	{
		NxF32 coeff = hitActor.getMass() * hit.length * 5.0f;
		hitActor.addForceAtLocalPos(hit.dir*coeff, NxVec3(0, 0, 0), NX_IMPULSE);
	}

	return NX_ACTION_NONE;
}

NxControllerAction  CControllerReport::onControllerHit( const NxControllersHit& hit )
{
	NxActor* actor = hit.controller->getActor();
	NxActor* hitActor = hit.other->getActor();
	NxCollisionGroup group = actor->getGroup();

	// 점프체크
	if ((COL_PLAYER1 | COL_PLAYER2 | COL_PLAYER3 | COL_PLAYER4) & group)
	{
		STATE eState = ((Player*)actor->userData)->getFSM()->GetCurrentState();
		switch (eState)
		{
		case STATE_JUMP:
			((Player*)actor->userData)->getAnimator()->Play();
			break;
		default:
			break;
		}
	}

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
				// 속도체크
				NxVec3 velocity = pair.actors[iBananaIndex]->getLinearVelocity();
				const float minimunVelocity = 50.0f;
				if (velocity.magnitude() > minimunVelocity)
				{
					float fDamage = ((CBanana*)pair.actors[iBananaIndex]->userData)->m_pPlayer->getDamage();
					((Player*)pair.actors[iNoBananaIndex]->userData)->proceedBeaten(fDamage);

				}
				
			}
			pair.actors[iBananaIndex]->setLinearVelocity(NxVec3(0.0f, 0.0f, 0.0f));
			( ( CBanana* )pair.actors[ iBananaIndex ]->userData )->Frozen();
		}
	}
}