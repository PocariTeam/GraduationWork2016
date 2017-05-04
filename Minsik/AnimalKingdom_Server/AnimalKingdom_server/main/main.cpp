#include "stdafx.h"

int main()
{
	CAnimationMgr::getInstance().Load( "../Executable/Animations.txt" );
	PhysXManager::getInstance().initPhysX();
	IOCPServer::getInstance().run();
	return 0;
}