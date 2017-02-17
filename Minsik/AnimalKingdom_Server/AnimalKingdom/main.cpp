#include "stdafx.h"
#include <crtdbg.h>

int main()
{
	IOCPServer::getInstance().run();
	return 0;
}