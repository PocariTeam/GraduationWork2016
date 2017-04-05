#include "stdafx.h"
#include "SessionManager.h"

SessionManager::SessionManager()
	: lock_(L"SessionManager")
{
	idSeed_ = 1;
}

SessionManager::~SessionManager()
{
	vector<Session *> removeSessionVec;
	removeSessionVec.resize(sessionList_.size());
	std::copy(sessionList_.begin(), sessionList_.end(), removeSessionVec.begin());
	for (auto session : removeSessionVec) {
		if (!this->closeSession(session)) {
			SLog(L"* Session[%d] close fail!",session->getID());
		}
	}
	sessionList_.clear();
	SLog(L"* ~SessionManager()");
}

bool SessionManager::addSession(Session * session)
{
	SAFE_LOCK(lock_);
	auto findSession = std::find(sessionList_.begin(), sessionList_.end(), session);
	if (findSession != sessionList_.end()) {
		return false;
	}
	if (sessionCount_ > SESSION_CAPACITY) {
		SLog(L"* session so busy. count[%d]", sessionCount_);
		return false;
	}

	session->setID(++(this->idSeed_));
	sessionList_.push_back(session);
	++sessionCount_;
	return true;
}

//������ ������� Ŭ���̾�Ʈ���� �����ϴ�.
bool SessionManager::closeSession(Session *session)
{
	SAFE_LOCK(lock_);
	if (session == nullptr) {
		return false;
	}
	auto findSession = std::find(sessionList_.begin(), sessionList_.end(), session);
	if (findSession != sessionList_.end()) {
		Session *delSession = *findSession;
		SLog(L"* detected close by client id[%d] {%S}", delSession->getID(),delSession->getAddress().c_str());
		::closesocket(delSession->getSocket());

		sessionList_.remove(delSession);
		--sessionCount_;
		SAFE_DELETE(delSession);
		return true;
	}
	return false;
}

//������ ������ �ݽ��ϴ�.
void SessionManager::forceCloseSession(Session *session)
{
	SAFE_LOCK(lock_);
	if (!session) {
		return;
	}

	//����� ���� ����. ������ Ŭ���̾�Ʈ���� ���� ������ �����ϵ��� �����ؾ� �Ѵ�.
	LINGER linger;
	linger.l_onoff = 1;   //���
	linger.l_linger = 0;  //���ð�, 0�Ͻ� �Ϸ� �ȵ� ��Ŷ ������ ��� ����.

	::setsockopt(session->getSocket(), SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger));
	this->closeSession(session);
}
