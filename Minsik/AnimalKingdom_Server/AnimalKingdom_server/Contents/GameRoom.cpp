#include "stdafx.h"
#include "GameRoom.h"
#include "NxControllerManager.h"

GameRoom::GameRoom(UINT num)
	: lock_(L"GameRoom"), isPlaying_(false), playerCount_(0), roomNum_(num), updateTimerID_(-1), hasWinner_(false), crownOwnerID_(0), leftWinningTime_(CROWN_WIN_SEC)
{
}

GameRoom::~GameRoom()
{
	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		SAFE_DELETE(iter->second);
	}
	players_.clear();
}

BOOL GameRoom::enter(Session* session)
{
	SAFE_LOCK(lock_);

	if (isPlaying_)
	{
		SLog(L"! the [%d] room is playing now.", roomNum_);
		return false;
	}

	if (playerCount_ < PLAYER_CAPACITY)
	{
		BOOL isMaster;
		isMaster = (playerCount_ == 0) ? true : false;
		players_.insert(make_pair(session->getID(), new Player(session, roomNum_, isMaster)));
		++playerCount_;
		session->setRoomNumber(roomNum_);
		SLog(L"* id[%d] enters the [%d] room. ", session->getID(),roomNum_ );
		return true;
	}
	else
	{
		SLog(L"! PlayerCount Overflow, currentCount: %d", playerCount_);
		return false;
	}
}

BOOL GameRoom::startRoom(Session* session)
{
	SAFE_LOCK(lock_);

	if (isPlaying_)
	{
		SLog(L"! the [%d] room is already playing now. ", roomNum_);
		return false;
	}

	if (playerCount_ < MINIMUM_START_COUNT)
	{
		SLog(L"! the [%d] room's the number of player is under %d(minimum). ", roomNum_, MINIMUM_START_COUNT);
		return false;
	}
		
	bool startCheck = false;
	auto findPlayer = players_[session->getID()];

	for (auto p = players_.begin(); p != players_.end(); p++)
	{
		if ( (p->second)->getMaster() && p->first == session->getID())
		{
			startCheck = true;
		}
		else if ((p->second)->getReady() == false)
		{
			SLog(L"! id[%d] is not ready in the [%d] room. ", session->getID(), roomNum_);
			return false;
		}
	}

	if (startCheck)
	{
		BOOL loadCheck = PhysXManager::getInstance().LoadSceneFromFile(session->getRoomNumber(), &players_ );
		if (loadCheck && GameRoom::setupGame())
		{
			isPlaying_ = true;
			SLog(L"* the [%d] room is starting now. ", roomNum_);
			return true;
		}
		else
		{
			return false;
		}
	}

	SLog(L"! id[%d] is not master of the [%d] room. ", session->getID(), roomNum_);
	return false;
}

void GameRoom::startGame()
{
	// 업데이트 시작
	TIMECAPS caps;
	timeGetDevCaps(&caps, sizeof(caps));
	updateTimerID_ = timeSetEvent((UINT)(UPDATE_TIME_SEC * 1000), caps.wPeriodMin, (LPTIMECALLBACK)updateTimer, roomNum_, TIME_PERIODIC);

	sendStartGame();

	// 스타트 이후부터 승자체크 ( 그 사이 나간 사람이 있는지 한번 체크 )
	hasWinner_ = false;
	if (MINIMUM_START_COUNT > 1)
	{
		checkWinner(false);
	}

}

BOOL GameRoom::exit(Session* session)
{
	SAFE_LOCK(lock_);

	auto find_iter = players_.find(session->getID());
	if (find_iter == players_.end())
	{
		SLog(L"! id[%d] doens't exists in the [%d] room. ", session->getID(), roomNum_);
		return false;
	}

	// 체력 0
	if (isPlaying_ && hasWinner_ == false)
	{
		find_iter->second->setHp(0);
		find_iter->second->setState(STATE::STATE_DOWN);
		checkWinner(false);
		sendPlayerSync();
	}
	

	SAFE_DELETE(find_iter->second);
	players_.erase(find_iter);
	--playerCount_;

	if (playerCount_ > 0)
	{
		(players_.begin()->second)->setMaster(true);
	}
	else if (playerCount_ == 0 && isPlaying_)
	{
		IOCPServer::getInstance().pushEvent(new event_obj{ roomNum_, this }, GetTickCount() + 1000, EVENT_FINISH);
	}

	PacketManager::getInstance().sendPlayerList(session->getRoomNumber());
	session->setRoomNumber(NOT_IN_ROOM);


	SLog(L"* id[%d] exits from the [%d] room. ", session->getID(), roomNum_);

	return true;

}

BOOL GameRoom::setPlayerReady(Session* session, BOOL b)
{
	SAFE_LOCK(lock_);
	
	if (isPlaying_)
	{
		SLog(L"! the [%d] room is playing now. ", roomNum_);
		return false;
	}

	auto find_iter = players_.find(session->getID());
	if (find_iter == players_.end())
	{
		SLog(L"! id[%d] doens't exists in the [%d] room. ", session->getID(), roomNum_);
		return false;
	}

	if ((find_iter->second)->getMaster())
	{
		SLog(L"! id[%d] is a master of the [%d] room. so it cannot be ready. ", session->getID(), roomNum_);
		return false;
	}
	(find_iter->second)->setReady(b);
	SLog(L"* id[%d] got ready[%d] in the [%d] room. ", session->getID(), b, roomNum_);
	return true;

}

BOOL GameRoom::setPlayerCharacter(Session* session, CHARACTER ch)
{
	SAFE_LOCK(lock_);

	if (isPlaying_)
	{
		SLog(L"! the [%d] room is playing now. ", roomNum_);
		return false;
	}

	auto find_iter = players_.find(session->getID());
	if (find_iter == players_.end())
	{
		SLog(L"! id[%d] doens't exists in the [%d] room. ", session->getID(), roomNum_);
		return false;
	}


	if ((find_iter->second)->getReady())
	{
		SLog(L"! id[%d] is ready now. so it cannot be changed.", session->getID());
		return false;
	}

	if (ch < 0 || ch > CHARACTER_MAX)
	{
		SLog(L"! wrong character. inputCharacter: %d", ch);
		return false;
	}

	(find_iter->second)->setCharacter(ch);
	SLog(L"* id[%d] chose character[%d] in the [%d] room. ", session->getID(), ch, roomNum_);
	return true;
}

RoomInfo GameRoom::getRoomInfo()
{
	SAFE_LOCK(lock_);
	RoomInfo r;
	r.playerCount = playerCount_;
	r.playing = isPlaying_;
	return r;
}

BOOL GameRoom::moveRequest(Session* session, Vector3 vDir)
{
	SAFE_LOCK(lock_);

	if (isPlaying_ == false) return true;

	auto find_iter = players_.find(session->getID());
	if (find_iter == players_.end())
	{
		SLog(L"! id[%d] doens't exists in the [%d] room. ", session->getID(), roomNum_);
		return false;
	}

	((find_iter->second))->setMoveDir(vDir);
	return true;
}

BOOL GameRoom::stateRequest(Session * session, STATE state)
{
	SAFE_LOCK(lock_);

	if (isPlaying_ == false) return true;

	auto find_iter = players_.find(session->getID());
	if (find_iter == players_.end())
	{
		SLog(L"! id[%d] doens't exists in the [%d] room. ", session->getID(), roomNum_);
		return false;
	}

	STATE cur = ((find_iter->second))->getFSM()->GetCurrentState();
	if (cur != STATE_DOWN && cur != STATE_DEAD)
	{
		((find_iter->second))->setState(state);
	}
	return true;
}

void GameRoom::sendPlayerList()
{
	SAFE_LOCK(lock_);
	PlayerInfo pList[PLAYER_CAPACITY] = { 0 };

	int i = 0;
	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		pList[i] = (iter->second)->getPlayerInfo();
		i++;
	}

	if (i != playerCount_)
	{
		SLog(L"! the [%d] room's playerCount_ is not correct. ",roomNum_);
		return;
	}

	S_PlayerList packet;
	packet.header.packetID = PAK_ID::PAK_ANS_PlayerList;
	packet.header.size = sizeof(packet);
	packet.playerCount = playerCount_;
	packet.roomNum = roomNum_;
	memcpy(packet.playerInfo, pList, sizeof(PlayerInfo)*PLAYER_CAPACITY);
	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		(iter->second)->getSession()->send((char*)&packet);
	}
	
}

void GameRoom::sendReadyGame()
{
	SAFE_LOCK(lock_);

	HEADER packet;
	packet.packetID = PAK_ID::PAK_ANS_ReadyGame;
	packet.size = sizeof(packet);
	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		(iter->second)->getSession()->send((char*)&packet);
	}
}

void GameRoom::sendStartGame()
{
	SAFE_LOCK(lock_);

	HEADER packet;
	packet.packetID = PAK_ID::PAK_ANS_StartGame;
	packet.size = sizeof(packet);
	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		(iter->second)->getSession()->send((char*)&packet);
	}
}

void GameRoom::sendWinner(int winner_id)
{
	SAFE_LOCK(lock_);

	S_Winner packet;
	packet.header.packetID = PAK_ID::PAK_ANS_Winner;
	packet.header.size = sizeof(packet);
	packet.id = winner_id;

	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		(iter->second)->getSession()->send((char*)&packet);
	}

	IOCPServer::getInstance().pushEvent(new event_obj{ roomNum_, this }, GetTickCount() + GAME_FINISH_DELAY, EVENT_FINISH);
}

void GameRoom::sendFinishGame()
{
	SAFE_LOCK(lock_);

	HEADER packet;
	packet.packetID = PAK_ID::PAK_ANS_FinishGame;
	packet.size = sizeof(packet);
	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		(iter->second)->getSession()->send((char*)&packet);
	}
}

void GameRoom::sendGetCrown(Player* player)
{
	SAFE_LOCK(lock_);

	int id = 0;

	if (player != nullptr)
	{
		for (auto iter = players_.begin(); iter != players_.end(); iter++)
		{
			if (iter->second == player)
			{
				id = iter->first;
				break;
			}
		}
	}

	crownOwnerID_ = id;

	S_Crown packet;
	packet.header.packetID = PAK_ID::PAK_ANS_Crown;
	packet.header.size = sizeof(packet);
	packet.id = crownOwnerID_;

	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		(iter->second)->getSession()->send((char*)&packet);
	}

}

void GameRoom::sendUseSkill(int id, bool use)
{
	SAFE_LOCK(lock_);

	if (players_.find(id) == players_.end())  return;

	S_UseSkill packet;
	packet.header.packetID = PAK_ID::PAK_ANS_UseSkill;
	packet.header.size = sizeof(packet);
	packet.id = id;
	packet.use = (BOOL)use;

	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		(iter->second)->getSession()->send((char*)&packet);
	}
}

void GameRoom::sendDefendEnd(int id)
{
	SAFE_LOCK(lock_);

	if (players_.find(id) == players_.end())  return;

	S_DefendEnd packet;
	packet.header.packetID = PAK_ID::PAK_ANS_DefendEnd;
	packet.header.size = sizeof(packet);
	packet.id = id;

	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		(iter->second)->getSession()->send((char*)&packet);
	}
}


BOOL GameRoom::setupGame()
{
	SAFE_LOCK(lock_);

	NxControllerManager* cctManager = PhysXManager::getInstance().getCCTManager(roomNum_);

	if (cctManager->getNbControllers() != playerCount_)
	{
		SLog(L"! nbControllers is not equal to playerCount_. ");
		return false;
	}

	int i = 0;
	for (auto p = players_.begin(); p != players_.end(); p++)
	{
		NxController* pController = cctManager->getController(i);
		CHARACTER character = (p->second)->getPlayerInfo().character;

		pController->getActor()->setGroup( COL_GROUP( COL_PLAYER1 << i ) );
		(p->second)->setCCT(pController);
		(p->second)->setAnimator(CAnimationMgr::getInstance().Clone(character));
		( p->second )->setReady( FALSE );
		(p->second)->initialize();
		i++;
	}

	leftPlayingTime_ = GAME_PLAYING_SEC;
	leftWinningTime_ = CROWN_WIN_SEC;
	hasWinner_ = true; // 후에 start 패킷 보낸 이후에 false로 전환
	crownOwnerID_ = 0;

	TIMECAPS caps;
	timeGetDevCaps(&caps, sizeof(caps));
	syncTimerID_ = timeSetEvent((UINT)(SYNC_TIME_SEC*1000), caps.wPeriodMin, (LPTIMECALLBACK)syncTimer, roomNum_, TIME_PERIODIC);
	
	IOCPServer::getInstance().pushEvent(new event_obj{ roomNum_, this }, GetTickCount() + GAME_START_DELAY, EVENT_START);

	return true;
}

void CALLBACK GameRoom::updateTimer(UINT , UINT, DWORD_PTR roomNum, DWORD_PTR, DWORD_PTR)
{
	PhysXManager::getInstance().updateScene((UINT)roomNum, UPDATE_TIME_SEC);
	RoomManager::getInstance().update( (UINT)roomNum, UPDATE_TIME_SEC);
}

void CALLBACK GameRoom::syncTimer(UINT, UINT, DWORD_PTR roomNum, DWORD_PTR, DWORD_PTR)
{
	RoomManager::getInstance().sendPlayerSync((UINT)roomNum);
	RoomManager::getInstance().sendDynamicSync((UINT)roomNum);
}

void GameRoom::update( float fTimeDelta )
{
	if (hasWinner_ == false)
	{
		// 왕관 남은 시간 체크
		if (crownOwnerID_ != 0)
		{
			leftWinningTime_ -= fTimeDelta;
			if (leftWinningTime_ < 0.0f)
			{
				leftWinningTime_ = 0.f;
				checkWinner(true);
			}
		}
		else
		{
			// 게임 플레이 시간 체크
			leftPlayingTime_ -= fTimeDelta;

			if (leftPlayingTime_ < 0.0f)
			{
				leftPlayingTime_ = 0.0f;
				checkWinner(true);
			}
		}
	}
	

	for(auto iter = players_.begin(); iter != players_.end(); ++iter )
		(iter->second)->update( fTimeDelta );
}

void GameRoom::sendPlayerSync()
{
	SAFE_LOCK(lock_);

	S_SyncPlayer playerPacket;
	playerPacket.header.packetID = PAK_ID::PAK_ANS_SyncPlayer;
	playerPacket.header.size = sizeof(playerPacket);
	playerPacket.playingTime = leftPlayingTime_;
	playerPacket.winningTime = leftWinningTime_;

	int i = 0;
	for (auto iter = players_.begin(); iter != players_.end(); iter++, i++)
	{
		playerPacket.playerPositions[i].id = (iter->second)->getSession()->getID();
		NxVec3 p = (iter->second)->getCCT()->getActor()->getGlobalPosition();
		playerPacket.playerPositions[i].position = Vector3(p.x, p.y, p.z);
		playerPacket.playerPositions[i].rotY = (iter->second)->getRotateY();
		playerPacket.playerPositions[i].state = (iter->second)->getFSM()->GetCurrentState();
		playerPacket.playerPositions[i].hp = (iter->second)->getHp();
		playerPacket.playerPositions[i].canUseSkill = (iter->second)->getCanUseSkill();
	}
	
	
	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		(iter->second)->getSession()->send((char*)&playerPacket);
	}

}

void GameRoom::sendDynamicSync()
{
	SAFE_LOCK(lock_);

	S_SyncDynamic *dynamicPacket = PhysXManager::getInstance().getDynamicInfo(roomNum_);

	if (dynamicPacket == NULL)	return;

	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		(iter->second)->getSession()->send((char*)dynamicPacket);
	}
	SAFE_DELETE(dynamicPacket);
}

void GameRoom::sendDynamicOneSync(NxActor*actor)
{
	SAFE_LOCK(lock_);

	S_SyncDynamicOne *dynamicPacket = PhysXManager::getInstance().getDynamicOneInfo(actor);
	
	if (dynamicPacket == NULL)	return;

	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		(iter->second)->getSession()->send((char*)dynamicPacket);
	}
	SAFE_DELETE(dynamicPacket);
}

BOOL GameRoom::getPlaying()
{
	SAFE_LOCK(lock_);
	return isPlaying_;
}

void GameRoom::checkWinner(bool bTimeOut)
{
	if (hasWinner_) return;

	if (players_.empty()) return;

	int winner_id = players_.begin()->first;
	int aliveCount = playerCount_;
	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		if ((iter->second)->getHp() <= 0) --aliveCount;
		else if (players_[winner_id]->getHp() < (iter->second)->getHp())
		{
			winner_id = iter->first;
		}
	}

	if (aliveCount == 0) // 생존자 0명
	{
		hasWinner_ = true;
		sendWinner(0);
		SLog(L"* the room [%d] game draw.. winner: nobody(0) ", roomNum_);
		for (auto iter = players_.begin(); iter != players_.end(); iter++)
		{
			iter->second->setHp(0);
			iter->second->getFSM()->Change_State(STATE_DOWN);
		}
	}
	else if (aliveCount == 1) // 생존자 1명
	{
		hasWinner_ = true;
		sendWinner(winner_id);
		SLog(L"* the winner is id[%d] in the [%d] room ", winner_id, roomNum_);
		for (auto iter = players_.begin(); iter != players_.end(); iter++)
		{
			if (iter->first == winner_id) continue;
			iter->second->setHp(0);
			iter->second->getFSM()->Change_State(STATE_DOWN);
		}
	}
	else if (bTimeOut) // 타임아웃
	{
		hasWinner_ = true;
		if (crownOwnerID_ != 0)
		{
			winner_id = crownOwnerID_;
		}

		for (auto iter = players_.begin(); iter != players_.end(); iter++)
		{
			if (iter->first == winner_id) continue;
			iter->second->setHp(0);
			iter->second->getFSM()->Change_State(STATE_DOWN);
		}
		sendWinner(winner_id);
		SLog(L"* TIME OUT:: the winner is id[%d] in the [%d] room ", winner_id, roomNum_);
	}

}

void GameRoom::loseCrown(Player *player)
{
	SAFE_LOCK(lock_);

	auto find_iter = players_.find(crownOwnerID_);
	if (find_iter == players_.end()) return;
	if (find_iter->second != player) return;

	leftWinningTime_ = CROWN_WIN_SEC;
	sendGetCrown(nullptr);
	player->powerDown();

	NxMat34 mtxAnimation = player->getAnimator()->GetCurrentAnimationMatrix(player, "Head");
	NxMat33 mtxRotation{};
	NxVec3	vOffset{};

	switch (player->GetCharacterType())
	{
	case CHARACTER_CHM:
		mtxRotation.rotX(0);
		vOffset = NxVec3(0.f, 9.75f + 2.5f, 0.f);
		break;
	case CHARACTER_MON:
		mtxRotation.rotX(XM_PI);
		vOffset = NxVec3(0.f, -0.7f - 0.2f, -0.05f);
		break;
	case CHARACTER_BAT:
		mtxRotation.rotX( 0 );
		vOffset = NxVec3(0.f, 9.75f + 2.5f, 5.f);
		break;
	default:
		SLog(L"undefined character type!");
		break;
	}
	NxMat34 mtxLocal{ mtxRotation, vOffset };

	PhysXManager::getInstance().missCrown(roomNum_, mtxAnimation * mtxLocal);
}

void GameRoom::finishGame()
{
	SAFE_LOCK(lock_);

	timeKillEvent(updateTimerID_);
	timeKillEvent(syncTimerID_);

	IOCPServer::getInstance().pushEvent(new event_obj{ roomNum_, this }, GetTickCount() + 1000, EVENT_RELEASE);
	
}

void GameRoom::setPlaying(bool b)
{
	SAFE_LOCK(lock_);

	isPlaying_ = b;
}

BOOL GameRoom::hasWinner()
{
	return hasWinner_;
}
