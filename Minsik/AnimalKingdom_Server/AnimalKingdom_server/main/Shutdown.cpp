#include "stdafx.h"
#include "Shutdown.h"

bool _shutdown = false;
void shutdownServer()
{
	SLog(L"### server shutdown!!! ###");
	_shutdown = true;

	//���� ������ ��� ������ ���� ����
	// SessionManager::getInstance().~SessionManager();
	
	//���� ������ ���� (��ŷ�̳� ...)
	//std::thread core();
	//core.join();
	//DB Ŀ�ؼ� ����
}