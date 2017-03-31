#include "stdafx.h"

int main()
{
	CAnimationMgr::getInstance().Load( "Animations.txt" );
	PhysXManager::getInstance().initPhysX();
	IOCPServer::getInstance().run();
	return 0;
}