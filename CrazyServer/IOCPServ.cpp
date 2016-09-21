// IOCPServ.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "BSocket.h"

int _tmain(int argc, _TCHAR* argv[])
{
	BSocket server;
	server.iniServer(2016);

	printf("hello");

	return 0;
}

