#include "stdafx.h"
#include <crtdbg.h>

int main()
{
	// AnimationMgr �Ҹ��ڸ� ���� Release �� �ҷ���
	CAnimationMgr::getInstance().Load( "Animations.txt" );
	PhysXManager::getInstance().initPhysX();
	IOCPServer::getInstance().run();

	return 0;
}