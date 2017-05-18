#include "stdafx.h"


int main()
{
	if (PhysXManager::getInstance().initPhysX() == false)	return 0;

	IOCPServer::getInstance().run();

	return 0;
}