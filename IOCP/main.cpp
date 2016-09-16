#include "stdafx.h"
#include "NetWork.h"

CNetwork gNetwork;

int main()
{
	//네트워크를 초기화 한다
	gNetwork.initNetwork();
	//서버를 시작한다
	gNetwork.startServer();
	//서버를 종료한다
	gNetwork.endServer();
}
