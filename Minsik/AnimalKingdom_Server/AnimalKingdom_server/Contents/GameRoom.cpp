#include "stdafx.h"
#include "GameRoom.h"
#include "NxControllerManager.h"

GameRoom::GameRoom(UINT num)
	: lock_(L"GameRoom"), isPlaying_(false), playerCount_(0), roomNum_(num), updateTimerID_(-1)
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

BOOL GameRoom::startGame(Session* session)
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
		BOOL loadCheck = PhysXManager::getInstance().LoadSceneFromFile(session->getRoomNumber());
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

BOOL GameRoom::exit(Session* session)
{
	SAFE_LOCK(lock_);

	auto find_iter = players_.find(session->getID());
	if (find_iter == players_.end())
	{
		SLog(L"! id[%d] doens't exists in the [%d] room. ", session->getID(), roomNum_);
		return false;
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
		timeKillEvent(updateTimerID_);
		timeKillEvent(syncTimerID_);
		PhysXManager::getInstance().ReleaseScene(roomNum_);
		isPlaying_ = false;
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

BOOL GameRoom::moveRequest(Session* session, Vector3 vDir, STATE state)
{
	SAFE_LOCK(lock_);

	auto find_iter = players_.find(session->getID());
	if (find_iter == players_.end())
	{
		SLog(L"! id[%d] doens't exists in the [%d] room. ", session->getID(), roomNum_);
		return false;
	}

	((find_iter->second))->setMoveDir_State(vDir, state);
	sendMovePacket(session->getID(), vDir, state);
	return true;


	return false;
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
	}

	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		Session* session = (iter->second)->getSession();
		S_PlayerList packet;
		packet.header.packetID = PAK_ID::PAK_ANS_PlayerList;
		packet.header.size = sizeof(S_PlayerList);
		packet.playerCount = playerCount_;
		packet.roomNum = roomNum_;
		memcpy(packet.playerInfo, pList, sizeof(PlayerInfo)*PLAYER_CAPACITY);
		session->send((char*)&packet);
	}
	
}

void GameRoom::sendMovePacket(UINT id, Vector3 vDir, STATE state)
{
	SAFE_LOCK(lock_);

	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		Session* session = (iter->second)->getSession();
		S_Move packet;
		packet.header.packetID = PAK_ID::PAK_ANS_Move;
		packet.header.size = sizeof(S_Move);
		packet.id = id;
		packet.vDir = vDir;
		packet.state = state;
		session->send((char*)&packet);
	}
}

void GameRoom::sendStartGame()
{
	SAFE_LOCK(lock_);

	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		Session* session = (iter->second)->getSession();
		HEADER packet;
		packet.packetID = PAK_ID::PAK_ANS_StartGame;
		packet.size = sizeof(packet);
		session->send((char*)&packet);
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
		NxController* pController = cctManager->getController(i++);
		UINT iActorCnt{};
		CHARACTER character = (p->second)->getPlayerInfo().character;

		NxActor** dpActors = PhysXManager::getInstance().CreateCharacterActors(character, roomNum_, iActorCnt);
		(p->second)->setActorArray(dpActors, iActorCnt);
		(p->second)->setCCT(pController);
		(p->second)->setAnimator(CAnimationMgr::getInstance().Clone(character));
	}

	TIMECAPS caps;
	timeGetDevCaps(&caps, sizeof(caps));
	updateTimerID_ = timeSetEvent(UPDATE_TIME_SEC*1000, caps.wPeriodMin, (LPTIMECALLBACK)updateTimer, roomNum_, TIME_PERIODIC);
	syncTimerID_ = timeSetEvent(SYNC_TIME_SEC*1000, caps.wPeriodMin, (LPTIMECALLBACK)syncTimer, roomNum_, TIME_PERIODIC);
	
	return true;
}

void CALLBACK GameRoom::updateTimer(UINT , UINT, DWORD_PTR roomNum, DWORD_PTR, DWORD_PTR)
{
	PhysXManager::getInstance().updateScene((UINT)roomNum, UPDATE_TIME_SEC);
	RoomManager::getInstance().update( (UINT)roomNum, UPDATE_TIME_SEC);
}

void CALLBACK GameRoom::syncTimer(UINT, UINT, DWORD_PTR roomNum, DWORD_PTR, DWORD_PTR)
{
	RoomManager::getInstance().sendSync((UINT)roomNum);
}

void GameRoom::update( float fTimeDelta )
{
	for(auto iter = players_.begin(); iter != players_.end(); ++iter )
		(iter->second)->update( fTimeDelta );
}

void GameRoom::sendSync()
{
	SAFE_LOCK(lock_);

	S_SyncPlayer playerPacket;
	playerPacket.header.packetID = PAK_ID::PAK_ANS_SyncPlayer;
	playerPacket.header.size = sizeof(playerPacket);
	int i = 0;
	for (auto iter = players_.begin(); iter != players_.end(); iter++, i++)
	{
		playerPacket.playerPositions[i].id = (iter->second)->getSession()->getID();
		NxVec3 p = (iter->second)->getCCT()->getActor()->getGlobalPosition();
		playerPacket.playerPositions[i].position = Vector3(p.x, p.y, p.z);
		playerPacket.playerPositions[i].rotY = (iter->second)->getRotateY();
	}
	
	S_SyncDynamic dynamicPacket = PhysXManager::getInstance().getDynamicInfo(roomNum_);

	for (auto iter = players_.begin(); iter != players_.end(); iter++)
	{
		Session* session = (iter->second)->getSession();
		session->send((char*)&playerPacket);
		session->send((char*)&dynamicPacket);
	}


}
