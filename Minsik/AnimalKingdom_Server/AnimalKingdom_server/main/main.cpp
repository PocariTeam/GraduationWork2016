#include "stdafx.h"
#include <crtdbg.h>

int main()
{
	// AnimationMgr 소멸자를 만들어서 Release 를 불러라
	CAnimationMgr::getInstance().Load( "Animations.txt" );
	PhysXManager::getInstance().initPhysX();
	IOCPServer::getInstance().run();

	return 0;
}