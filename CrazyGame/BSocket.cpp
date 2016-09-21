#include "stdafx.h"
#include "BSocket.h"
#include "BGame.h"
#include <process.h>

BSocket::BSocket(void)
{
}


BSocket::~BSocket(void)
{
}


//

int recvn(SOCKET s, char* buf, int len, int flags)
{
	int readLen = 0;

	while(true){

		readLen += recv(s, &buf[readLen], len, 0);
		if(readLen >= len){

			break;
		}
	}

	return 0;
}


unsigned int WINAPI EchoThreadMain(LPVOID game){

	BSocket* bSock = (BSocket*)game;

	while(true){

		recvn(bSock->s, (char*)&bSock->head, sizeof(head), 0);
		
		/*switch(bSock->head.dataType){

		default:
	
			break;
		}*/
	}

	closesocket(bSock->s);
	WSACleanup();

	return 0;
}


void BSocket::createServer(char* ip, int port)
{
	//pGame = game;

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){

		exit(1);
	}

	s = socket(PF_INET, SOCK_STREAM, 0);
	if(s == INVALID_SOCKET){

		exit(1);
	}

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr(ip);
	saddr.sin_port = port;

	if(connect(s, (SOCKADDR*)&saddr, sizeof(saddr)) == SOCKET_ERROR){

		exit(1);
	}

	_beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)this, 0, NULL);	
}
