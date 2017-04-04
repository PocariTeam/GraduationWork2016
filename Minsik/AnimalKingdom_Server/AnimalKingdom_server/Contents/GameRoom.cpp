#include "stdafx.h"
#include "GameRoom.h"
#include "NxControllerManager.h"

GameRoom::GameRoom(UINT num)
	: lock_(L"GameRoom"), isPlaying_(false), playerCount_(0), roomNum_(num), updateTimerID_(-1)
{
}

GameRoom::~GameRoom()
{
	for (auto iter = playerList_.begin(); iter != playerList_.end(); iter++)
	{
		SAFE_DELETE(*iter);
	}
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
		playerList_.push_back(new Player(session, roomNum_, isMaster));
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
	for (auto p = playerList_.begin(); p != playerList_.end(); p++)
	{
		if ((*p)->getMaster() && (*p)->getSession() == session)
		{
			startCheck = true;
		}
		else if ((*p)->getReady() == false)
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

class SyncPacket : public Work
{
	void tick()
	{
		
	}
};


BOOL GameRoom::exit(Session* session)
{
	SAFE_LOCK(lock_);
	for (auto p = playerList_.begin(); p != playerList_.end(); p++)
	{
		if ((*p)->getSession() == session)
		{
			SLog(L"* id[%d] exits from the [%d] room. ", session->getID(), roomNum_);
			playerList_.remove(*p);
			--playerCount_;

			if (playerCount_ > 0)
			{
				(*playerList_.begin())->setMaster(true);
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

			return true;
		}
	}
	return false;
}

BOOL GameRoom::setPlayerReady(Session* session, BOOL b)
{
	SAFE_LOCK(lock_);

	if (isPlaying_)
	{
		SLog(L"! the [%d] room is playing now. ", roomNum_);
		return false;
	}

	for (auto p = playerList_.begin(); p != playerList_.end(); p++)
	{
		if ((*p)->getSession() == session)
		{
			if ((*p)->getMaster())
			{
				SLog(L"! id[%d] is a master of the [%d] room. so it cannot be ready. ", session->getID(), roomNum_);
				return false;
			}
			(*p)->setReady(b);
			SLog(L"* id[%d] got ready[%d] in the [%d] room. ", session->getID(), b, roomNum_);
			return true;
		}
	}
	SLog(L"! id[%d] is not in the [%d] room. ", session->getID(), roomNum_);

	return false;
}

BOOL GameRoom::setPlayerCharacter(Session* session, CHARACTER ch)
{
	SAFE_LOCK(lock_);

	if (isPlaying_)
	{
		SLog(L"! the [%d] room is playing now. ", roomNum_);
		return false;
	}

	for (auto p = playerList_.begin(); p != playerList_.end(); p++)
	{
		if ((*p)->getSession() == session)
		{
			if ((*p)->getReady())
			{
				SLog(L"! id[%d] is ready now. so it cannot be changed.", session->getID());
				return false;
			}
			if (ch < 0 || ch > CHARACTER_MAX)
			{
				SLog(L"! wrong character. inputCharacter: %d", ch);
				return false;
			}
			(*p)->setCharacter(ch);
			SLog(L"* id[%d] chose character[%d] in the [%d] room. ", session->getID(), ch, roomNum_);
			return true;
		}
	}
	SLog(L"! id[%d] is not in the [%d] room. ", session->getID(), roomNum_);

	return false;
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

	for (auto p = playerList_.begin(); p != playerList_.end(); p++)
	{
		if ((*p)->getSession() == session)
		{
			(*p)->setMoveDir_State(vDir, state);
			sendMovePacket(session->getID(), vDir, state);
			return true;
		}
	}

	return false;
}

void GameRoom::sendPlayerList()
{
	SAFE_LOCK(lock_);
	PlayerInfo pList[PLAYER_CAPACITY] = { 0 };

	int i = 0;
	for (auto iter = playerList_.begin(); iter != playerList_.end(); iter++)
	{
		pList[i] = (*iter)->getPlayerInfo();
		i++;
	}

	if (i != playerCount_)
	{
		SLog(L"! the [%d] room's playerCount_ is not correct. ",roomNum_);
	}

	for (auto iter = playerList_.begin(); iter != playerList_.end(); iter++)
	{
		Session* session = (*iter)->getSession();
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
	for (auto iter = playerList_.begin(); iter != playerList_.end(); iter++)
	{
		Session* session = (*iter)->getSession();
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

	for (auto iter = playerList_.begin(); iter != playerList_.end(); iter++)
	{
		Session* session = (*iter)->getSession();
		HEADER packet;
		packet.packetID = PAK_ID::PAK_ANS_StartGame;
		packet.size = sizeof(packet);
		session->send((char*)&packet);
	}
}

BOOL GameRoom::setupGame()
{
	NxControllerManager* cctManager = PhysXManager::getInstance().getCCTManager(roomNum_);

	if (cctManager->getNbControllers() != playerCount_)
	{
		SLog(L"! nbControllers is not equal to playerCount_. ");
		return false;
	}

	int i = 0;
	for (auto p = playerList_.begin(); p != playerList_.end(); p++)
	{
		NxController* pController = cctManager->getController(i++);
		UINT iActorCnt{};
		CHARACTER character = (*p)->getPlayerInfo().character;

		NxActor** dpActors = PhysXManager::getInstance().CreateCharacterActors(character, roomNum_, iActorCnt);
		(*p)->setActorArray(dpActors, iActorCnt);
		(*p)->setCCT(pController);
		(*p)->setAnimator(CAnimationMgr::getInstance().Clone(character));
	}

	TIMECAPS caps;
	timeGetDevCaps(&caps, sizeof(caps));
	updateTimerID_ = timeSetEvent(17, caps.wPeriodMin, (LPTIMECALLBACK)updateTimer, roomNum_, TIME_PERIODIC);
	syncTimerID_ = timeSetEvent(50, caps.wPeriodMin, (LPTIMECALLBACK)syncTimer, roomNum_, TIME_PERIODIC);

	return true;
}

void CALLBACK GameRoom::updateTimer(UINT , UINT, DWORD_PTR roomNum, DWORD_PTR, DWORD_PTR)
{
	PhysXManager::getInstance().updateScene((UINT)roomNum, 17.0f);
	RoomManager::getInstance().update( (UINT)roomNum, 17.0f );
}

void CALLBACK GameRoom::syncTimer(UINT, UINT, DWORD_PTR roomNum, DWORD_PTR, DWORD_PTR)
{
	RoomManager::getInstance().sendSync((UINT)roomNum);
}

void GameRoom::update( float fTimeDelta )
{
	for(auto iter = playerList_.begin(); iter != playerList_.end(); ++iter )
		(*iter)->update( fTimeDelta );
}

void GameRoom::sendSync()
{
//	static int check = 0;
//	printf("[%d] Sync \n", check++);

	S_Sync packet;
	packet.header.packetID = PAK_ID::PAK_ANS_Sync;
	packet.header.size = sizeof(packet);
	int i = 0;
	for (auto iter = playerList_.begin(); iter != playerList_.end(); iter++)
	{
		packet.playerPosition[i].id = (*iter)->getSession()->getID();
		NxVec3 p = (*iter)->getCCT()->getActor()->getGlobalPosition();
		printf("(%f, %f, %f) \n", p.x, p.y, p.z);
		packet.playerPosition[i].position = Vector3(p.x, p.y, p.z);
		packet.playerPosition[i].rotY = (*iter)->getRotateY();
	}
	
	for (auto iter = playerList_.begin(); iter != playerList_.end(); iter++)
	{
		Session* session = (*iter)->getSession();
		session->send((char*)&packet);
	}
}
