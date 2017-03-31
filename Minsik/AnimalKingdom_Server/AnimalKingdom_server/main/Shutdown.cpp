#include "stdafx.h"
#include "Shutdown.h"

bool _shutdown = false;
void shutdownServer()
{
	SLog(L"### server shutdown!!! ###");
	_shutdown = true;
}