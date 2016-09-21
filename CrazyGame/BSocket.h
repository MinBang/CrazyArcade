#pragma once
#include <WinSock2.h>
#include "gameProtocol.h"

#pragma comment(lib, "ws2_32")

class BSocket
{
public:

	char ip[128];
	int port;

	//sock
	WSADATA wsaData;
	SOCKADDR_IN saddr;
	SOCKET s;

	S_HEAD head;

public:
	BSocket(void);
	~BSocket(void);

	void createServer(char* ip, int port);
};

