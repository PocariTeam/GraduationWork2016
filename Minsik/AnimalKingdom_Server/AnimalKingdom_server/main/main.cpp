#include "stdafx.h"
#include <crtdbg.h>

int main()
{
	PhysXManager::getInstance().initPhysX();
	IOCPServer::getInstance().run();
	return 0;
}