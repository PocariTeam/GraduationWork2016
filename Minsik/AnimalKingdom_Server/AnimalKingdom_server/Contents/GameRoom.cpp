#include "stdafx.h"
#include "GameRoom.h"
#include "NxControllerManager.h"

GameRoom::GameRoom(UINT num)
	: lock_(L"GameRoom"), isPlaying_(false), playerCount_(0), roomNum_(num), timerID_(-1)
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
		SLog(L"* [%S] enters the [%d] room. ", session->getAddress().c_str(),roomNum_ );
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

	if (playerCount_ < 2 )
	{
		SLog(L"! the [%d] room's the number of player is under 2. ", roomNum_);
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
			SLog(L"! [%S] is not ready in the [%d] room. ", session->getAddress().c_str(), roomNum_);
			return false;
		}
	}

	if (startCheck)
	{
		SLog(L"* the [%d] room is starting now. ", roomNum_);
		isPlaying_ = true;
		return true;
	}

	SLog(L"! [%S] is not master of the [%d] room. ", session->getAddress().c_str(), roomNum_);
	return false;
}

BOOL GameRoom::setupGame()
{
	if (!isPlaying_)
	{
		SLog(L"! the [%d] room is not playing now. setupGame() error! ", roomNum_);
		return false;
	}

	NxControllerManager* cctManager = PhysXManager::getInstance().getCCTManager(roomNum_);

	if (cctManager->getNbControllers() != playerCount_)
	{
		SLog(L"! nbControllers is not equal to playerCount_. ");
		return false;
	}

	int i = 0;
	for (auto p = playerList_.begin(); p != playerList_.end(); p++)
	{
		(*p)->setCCT(cctManager->getController(i++));
	}

	return true;
	
}

BOOL GameRoom::exit(Session* session)
{
	SAFE_LOCK(lock_);
	for (auto p = playerList_.begin(); p != playerList_.end(); p++)
	{
		if ((*p)->getSession() == session)
		{
			SLog(L"* [%S] exits from the [%d] room. ", session->getAddress().c_str(), roomNum_);
			playerList_.remove(*p);
			--playerCount_;

			if (playerCount_ > 0)
			{
				(*playerList_.begin())->setMaster(true);
			}
			else if (playerCount_ == 0 && isPlaying_)
			{
				timeKillEvent(timerID_);
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
				SLog(L"! [%S] is a master of the [%d] room. so it cannot be ready. ", session->getAddress().c_str(), roomNum_);
				return false;
			}
			(*p)->setReady(b);
			SLog(L"* [%S] got ready[%d] in the [%d] room. ", session->getAddress().c_str(), b, roomNum_);
			return true;
		}
	}
	SLog(L"! [%S] is not in the [%d] room. ", session->getAddress().c_str(), roomNum_);

	return false;
}

BOOL GameRoom::setPlayerCharacter(Session* session, S_CHARACTER ch)
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
				SLog(L"! [%S] is ready now. so it cannot be changed.", session->getAddress().c_str());
				return false;
			}
			if (ch < 0 || ch > CHARACTER_MAX)
			{
				SLog(L"! wrong character. inputCharacter: %d", ch);
				return false;
			}
			(*p)->setCharacter(ch);
			SLog(L"* [%S] chose character[%d] in the [%d] room. ", session->getAddress().c_str(), ch, roomNum_);
			return true;
		}
	}
	SLog(L"! [%S] is not in the [%d] room. ", session->getAddress().c_str(), roomNum_);

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

BOOL GameRoom::moveRequest(Session* session, time_t tick, Vector3 vDir)
{
	SAFE_LOCK(lock_);

	for (auto p = playerList_.begin(); p != playerList_.end(); p++)
	{
		if ((*p)->getSession() == session)
		{
			NxU32	dwCollisionFlag;
			NxVec3	dir;
			dir.x = vDir.x; 
			dir.y = vDir.y; 
			dir.z = vDir.z;

			time_t difference = Clock::getInstance().systemTick() - tick;
			//dir *= difference;
			(*p)->getCCT()->move(dir, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag);
			
			sendMovePacket(session->getID(), tick, vDir);

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
		S_PlayerList* packet = (S_PlayerList*)session->ioData_[IO_SEND].buffer_.data();
		packet->header.packetID = PAK_ID::PAK_ANS_PlayerList;
		packet->header.size = sizeof(S_PlayerList);
		packet->playerCount = playerCount_;
		packet->roomNum = roomNum_;
		memcpy(packet->playerInfo, pList, sizeof(PlayerInfo)*PLAYER_CAPACITY);
		session->ioData_[IO_SEND].totalBytes_ = sizeof(S_PlayerList);
		session->send();
	}
	
}

void GameRoom::sendMovePacket(UINT32 id, time_t tick, Vector3 vDir)
{
	for (auto iter = playerList_.begin(); iter != playerList_.end(); iter++)
	{
		Session* session = (*iter)->getSession();
		S_Move* packet = (S_Move*)session->ioData_[IO_SEND].buffer_.data();
		packet->header.packetID = PAK_ID::PAK_ANS_Move;
		packet->header.size = sizeof(S_Move);
		packet->id = id;
		packet->tick = tick;
		packet->vDir = vDir;
		session->ioData_[IO_SEND].totalBytes_ = sizeof(S_Move);
		session->send();
	}
}

void GameRoom::sendStartGame()
{
	SAFE_LOCK(lock_);

	for (auto iter = playerList_.begin(); iter != playerList_.end(); iter++)
	{
		Session* session = (*iter)->getSession();
		S_StartGame* packet = (S_StartGame*)session->ioData_[IO_SEND].buffer_.data();
		packet->header.packetID = PAK_ID::PAK_ANS_StartGame;
		packet->header.size = sizeof(S_StartGame);
		packet->startTick = Clock::getInstance().systemTick();
		session->ioData_[IO_SEND].totalBytes_ = sizeof(S_StartGame);
		session->send();
	}

	TIMECAPS caps;
	timeGetDevCaps(&caps, sizeof(caps));
	timerID_ = timeSetEvent(17, caps.wPeriodMin, (LPTIMECALLBACK)updateTimer, roomNum_, TIME_PERIODIC);
}

void CALLBACK GameRoom::updateTimer(UINT , UINT, DWORD_PTR roomNum, DWORD_PTR, DWORD_PTR)
{
	PhysXManager::getInstance().updateScene(roomNum, 17);
}
