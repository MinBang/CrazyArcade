#include "stdafx.h"
#include "BSocket.h"


BSocket* pSock;
CRITICAL_SECTION mSc;

GAME_ROOM room[ROOM_SIZE];

char map_first[MAP_HEIGHT][MAP_WIDTH] = {
	7, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 7,
	5, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
	4, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 4,
	5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
	4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
	5, 0, 0, 0, 0, 0, 6, 6, 6, 0, 0, 0, 0, 0, 5,
	4, 0, 0, 0, 0, 0, 6, 6, 6, 0, 0, 0, 0, 0, 4,
	5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
	4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
	5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
	4, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 4,
	5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
	7, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 7	
};

BSocket::BSocket(void)
{
	recvBytes = 0;
	flags = 0;
	serial = 0;
	hLen = sizeof(head);
	pSock = this;

	InitializeCriticalSection( &mSc );

	initRoom();
}


BSocket::~BSocket(void)
{
	DeleteCriticalSection( &mSc );
}

void BSocket::setHeader(LPPER_IO_DATA ioInfo)
{
	memcpy(&head, ioInfo->buf, hLen);
}


void BSocket::errorHandling(char* msg)
{
}

unsigned int WINAPI ThreadMain(LPVOID CompletionPortIO)
{
	HANDLE hComPort = (HANDLE)CompletionPortIO;
	SOCKET sock;
	DWORD bytesTrans;
	LPPER_HANDLE_DATA handleInfo;
	LPPER_IO_DATA ioInfo;
	DWORD flags = 0;

	while(true){

		int re = GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPDWORD)&handleInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);
		//printf("¹ÝÈ¯°ª : %d\n", ioInfo->rwMode);
		sock = handleInfo->hClntSock;

		if(ioInfo->rwMode == READ){

			//puts("message received!");

			if(bytesTrans == 0){

				puts("user out!!");

				pSock->closeUser(sock);

				//shutdown(sock, SD_RECEIVE);
				closesocket(sock);
				free(handleInfo);
				free(ioInfo);			

				continue;
			}

			pSock->requestHandling(sock, ioInfo, flags);

			pSock->recvData(sock, ioInfo, flags);
			
		}else{

			//puts("message send!!");

			free(ioInfo);
		}
	}

	return 0;
}

BPlayer* BSocket::intoRoom(SOCKET s, S_AWAITE* pAwaite, BPlayer* pPlayer)
{
	int sx = 0;
	int sy = 0;

	for(int i=0;i<ROOM_SIZE;i++){
		
		if(room[i].isPlay == false){

			for(int j=0;j<MAX_USER_INROOM;j++){

				if(room[i].player[j].serial == -1){

					switch(j){

					case 0:

						sx = START_X + OBJECT_SIZE;
						sy = START_Y + OBJECT_SIZE;

						room[i].player[j].x = sx;
						room[i].player[j].y = sy;

						break;

					case 1:

						sx = START_X + 13 * OBJECT_SIZE;
						sy = START_Y + OBJECT_SIZE;
						
						room[i].player[j].x = sx;
						room[i].player[j].y = sy;

						break;

					case 2:

						sx = START_X + OBJECT_SIZE;
						sy = START_Y + 10 * OBJECT_SIZE;
						
						room[i].player[j].x = sx;
						room[i].player[j].y = sy;

						break;

					case 3:

						sx = START_X + 13 * OBJECT_SIZE;
						sy = START_Y + 10 * OBJECT_SIZE;
						
						room[i].player[j].x = sx;
						room[i].player[j].y = sy;

						break;

					default:

						sx = 0;
						sy = 0;

						break;
					}

					room[i].player[j].serial = pAwaite->serial;
					strcpy_s(room[i].player[j].id, pAwaite->id);
					room[i].player[j].room = i;
					room[i].player[j].type = IMG_CHRT_BAZZIFACE + j;

					room[i].userCount++;
					room[i].sock[j] = s;

					return &room[i].player[j];
				}
			}
		}
	}

	return NULL;
}

LPPER_IO_DATA BSocket::createIoInfo(int rwMode)
{
	LPPER_IO_DATA tInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
	memset(&(tInfo->overlapped), 0, sizeof(OVERLAPPED));
	tInfo->wsaBuf.len = BUF_SIZE;
	tInfo->wsaBuf.buf = tInfo->buf;
	tInfo->rwMode = rwMode;

	return tInfo;
}

void BSocket::closeUser(SOCKET s)
{
	for(int i=0;i<ROOM_SIZE;i++){

		for(int j=0;j<MAX_USER_INROOM;j++){

			if(room[i].sock[j] == s){

				room[i].player[j].init();
				room[i].sock[j] = NULL;
				room[i].ready[j] = 0;
				room[i].userCount--;
			}
		}
	}
}

void BSocket::iniServer(int port)
{
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)){
		errorHandling("WSAStartup Error!!\n");
	}

	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	GetSystemInfo(&sysInfo);

	for(int i=0;i<sysInfo.dwNumberOfProcessors;i++){

		_beginthreadex(NULL, 0, ThreadMain, (LPVOID)hComPort, 0, NULL);
	}

	hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = port;

	bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr));
	listen(hServSock, 5);

	while(true){

		SOCKET hClntSock;
		SOCKADDR_IN clntAdr;
		int addrLen = sizeof(clntAdr);

		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);
		handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
		handleInfo->hClntSock = hClntSock;
		memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);

		puts("New User!!");

		serial++;

		CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (DWORD)handleInfo, 0);

		ioInfo = createIoInfo(READ);

		WSARecv(handleInfo->hClntSock, &(ioInfo->wsaBuf), 1, &recvBytes, &flags, &(ioInfo->overlapped), NULL);
	}
}

void BSocket::initRoom(void)
{
	for(int i=0;i<ROOM_SIZE;i++){

		for(int j=0;j<4;j++){

			room[i].ready[j] = FALSE;
			room[i].sock[j] = NULL;
		}
	
		ZeroMemory(room[i].map, sizeof(room[i].map));
		room[i].userCount = 0;
		room[i].isPlay = FALSE;
	}
}


void BSocket::recvData(SOCKET s, LPPER_IO_DATA ioInfo, DWORD flags)
{
	ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
	memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
	ioInfo->wsaBuf.len = BUF_SIZE;
	ioInfo->wsaBuf.buf = ioInfo->buf;
	ioInfo->rwMode = READ;

	WSARecv(s, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
}

// dev

void BSocket::requestHandling(SOCKET s, LPPER_IO_DATA ioInfo, int flags)
{
	setHeader(ioInfo);

	switch(head.dataType){

	case CS_GAMEDATA:

		sendGameDataRun(s, ioInfo, flags);

		break;

	case CS_CONTACT_FIRST:

		sendContact(s, ioInfo, flags);

		break;

	case CS_AWAITE:

		sendAwaite(s, ioInfo, flags);

		break;

	case CS_READY:

		sendReady(s, ioInfo, flags);

		break;

	case CS_GAMESTART:

		sendGameStart(s, ioInfo, flags);

		break;

	default:
		break;
	}
}


int BSocket::sendContact(SOCKET s, LPPER_IO_DATA ioInfo, int flags)
{
	S_RCONTACT sData;

	int dLen = sizeof(sData);

	head.dataType = SC_CONTACT_FIRST;
	head.dLen = dLen;

	sData.serial = serial;

	ZeroMemory(ioInfo->buf, sizeof(ioInfo->buf));
	memcpy(ioInfo->buf, &head, sizeof(head));
	memcpy(ioInfo->buf + hLen, &sData, dLen);	
	ioInfo->wsaBuf.len = hLen + dLen;

	memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
	ioInfo->rwMode = WRITE;
	WSASend(s, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);	

	return 0;
}


int BSocket::sendAwaite(SOCKET s, LPPER_IO_DATA ioInfo, int flags)
{
	S_AWAITE rData;
	BPlayer* pPlayer = NULL;
	memcpy(&rData, ioInfo->buf + hLen, sizeof(rData));

	pPlayer = intoRoom(s, &rData, pPlayer);
	int num = pPlayer->room;
	free(ioInfo);

	if(pPlayer == NULL){

		printf("full!!\n");
	}else{

		S_RAWAITE sData;
		int dLen = sizeof(sData);

		head.dataType = SC_AWAITE;
		head.dLen = dLen;

		for(int i=0;i<MAX_USER_INROOM;i++){

			strcpy_s(sData.awaiters[i].id, room[num].player[i].id);
			sData.awaiters[i].chrtType = room[num].player[i].type;
			sData.awaiters[i].ready = room[num].ready[i];
			sData.awaiters[i].serial = room[num].player[i].serial;
			sData.awaiters[i].room = num;
		}

		for(int i=0;i<MAX_USER_INROOM;i++){

			if(room[num].sock != NULL && room[num].player[i].serial != -1){	

				LPPER_IO_DATA sInfo = createIoInfo(WRITE);

				ZeroMemory(sInfo->buf, sizeof(sInfo->buf));
				memcpy(sInfo->buf, &head, sizeof(head));
				memcpy(sInfo->buf + hLen, &sData, dLen);	
				sInfo->wsaBuf.len = hLen + dLen;

				WSASend(room[num].sock[i], &(sInfo->wsaBuf), 1, NULL, 0, &(sInfo->overlapped), NULL);								
			}
		}
	}	

	return 0;
}

int BSocket::sendReady(SOCKET s, LPPER_IO_DATA ioInfo, int flags)
{
	S_READY rData;
	memcpy(&rData, ioInfo->buf + hLen, sizeof(rData));
	int num = rData.roomNum;
	int readyCount = 0;

	for(int i=0;i<MAX_USER_INROOM;i++){

		if(room[num].player[i].serial == rData.serial){

			room[num].ready[i] += rData.ready;
			room[num].ready[i] = room[num].ready[i] % 2;

			break;
		}
	}

	free(ioInfo);

	if(2 <= room[num].userCount){

		for(int i=0;i<MAX_USER_INROOM;i++){

			readyCount += room[num].ready[i];
		}
	}

	S_RAWAITE sData;
	int dLen = sizeof(sData);

	head.dataType = SC_AWAITE;
	head.dLen = dLen;

	for(int i=0;i<MAX_USER_INROOM;i++){

		strcpy_s(sData.awaiters[i].id, room[num].player[i].id);
		sData.awaiters[i].chrtType = room[num].player[i].type;
		sData.awaiters[i].ready = room[num].ready[i];
		sData.awaiters[i].serial = room[num].player[i].serial;
		sData.awaiters[i].room = num;
	}

	for(int i=0;i<MAX_USER_INROOM;i++){

		if(room[num].sock != NULL && room[num].player[i].serial != -1){	

			LPPER_IO_DATA sInfo = createIoInfo(WRITE);

			ZeroMemory(sInfo->buf, sizeof(sInfo->buf));
			memcpy(sInfo->buf, &head, sizeof(head));
			memcpy(sInfo->buf + hLen, &sData, dLen);	
			sInfo->wsaBuf.len = hLen + dLen;

			WSASend(room[num].sock[i], &(sInfo->wsaBuf), 1, NULL, 0, &(sInfo->overlapped), NULL);								
		}
	}

	return 0;
}

int BSocket::sendGameStart(SOCKET s, LPPER_IO_DATA ioInfo, int flags)
{
	S_GAMESTART rData;
	memcpy(&rData, ioInfo->buf + hLen, sizeof(rData));

	free(ioInfo);

	int num = rData.room;
	int readyCount = 0;

	/*if(2 <= room[num].userCount){

		for(int i=0;i<MAX_USER_INROOM;i++){

			readyCount += room[num].ready[i];
		}
	}*/

	//if(room[num].userCount == readyCount){

		room[num].isPlay = TRUE;
		memcpy(room[num].map, map_first, sizeof(map_first));

		S_RGAMEDATA sData;
		int dLen = sizeof(sData);

		head.dataType = SC_GAMEDATA;
		head.dLen = dLen;

		memcpy(sData.map, room[num].map, sizeof(room[num].map));

		for(int i=0;i<MAX_USER_INROOM;i++){
			
			if(room[num].player[i].serial != -1){

				room[num].player[i].type -= 10;
			}

			sData.player[i] = room[num].player[i];
		}

		for(int i=0;i<MAX_USER_INROOM;i++){

			if(room[num].sock != NULL && room[num].player[i].serial != -1){	

				sData.num = i;

				LPPER_IO_DATA sInfo = createIoInfo(WRITE);

				ZeroMemory(sInfo->buf, sizeof(sInfo->buf));
				memcpy(sInfo->buf, &head, sizeof(head));
				memcpy(sInfo->buf + hLen, &sData, dLen);	
				sInfo->wsaBuf.len = hLen + dLen;

				WSASend(room[num].sock[i], &(sInfo->wsaBuf), 1, NULL, 0, &(sInfo->overlapped), NULL);								
			}
		}
	//}	

	return 0;
}

int BSocket::sendGameData(SOCKET s, LPPER_IO_DATA ioInfo, int flags)
{
	S_GAMESTART rData;
	memcpy(&rData, ioInfo->buf + hLen, sizeof(rData));

	free(ioInfo);

	int num = rData.room;

	S_RGAMEDATA sData;
	int dLen = sizeof(sData);

	head.dataType = SC_GAMEDATA;
	head.dLen = dLen;

	memcpy(sData.map, room[num].map, sizeof(room[num].map));
	
	for(int i=0;i<MAX_USER_INROOM;i++){

		sData.player[i] = room[num].player[i];
	}
	

	for(int i=0;i<MAX_USER_INROOM;i++){

		if(room[num].sock != NULL && room[num].player[i].serial != -1){	

			LPPER_IO_DATA sInfo = createIoInfo(WRITE);

			ZeroMemory(sInfo->buf, sizeof(sInfo->buf));
			memcpy(sInfo->buf, &head, sizeof(head));
			memcpy(sInfo->buf + hLen, &sData, dLen);	
			sInfo->wsaBuf.len = hLen + dLen;

			WSASend(room[num].sock[i], &(sInfo->wsaBuf), 1, NULL, 0, &(sInfo->overlapped), NULL);								
		}
	}	
		
	return 0;
}



int BSocket::sendGameDataRun(SOCKET s, LPPER_IO_DATA ioInfo, int flags)
{
	S_GAMEDATA rData;
	memcpy(&rData, ioInfo->buf + hLen, sizeof(rData));

	int num = rData.player.room;

	room[num].player[rData.num] = rData.player;

	S_RGAMEDATA sData;
	int dLen = sizeof(sData);

	head.dataType = SC_GAMEDATA;
	head.dLen = dLen;

	memcpy(sData.map, room[num].map, sizeof(room[num].map));
	
	for(int i=0;i<MAX_USER_INROOM;i++){

		sData.player[i] = room[num].player[i];
	}

	ZeroMemory(ioInfo->buf, sizeof(ioInfo->buf));
	memcpy(ioInfo->buf, &head, sizeof(head));
	memcpy(ioInfo->buf + hLen, &sData, dLen);	
	ioInfo->wsaBuf.len = hLen + dLen;

	memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
	ioInfo->rwMode = WRITE;
	WSASend(s, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);	

	/*for(int i=0;i<MAX_USER_INROOM;i++){

		if(room[num].sock != NULL && room[num].player[i].serial != -1){	

			LPPER_IO_DATA sInfo = createIoInfo(WRITE);

			ZeroMemory(sInfo->buf, sizeof(sInfo->buf));
			memcpy(sInfo->buf, &head, sizeof(head));
			memcpy(sInfo->buf + hLen, &sData, dLen);	
			sInfo->wsaBuf.len = hLen + dLen;

			WSASend(room[num].sock[i], &(sInfo->wsaBuf), 1, NULL, 0, &(sInfo->overlapped), NULL);								
		}
	}	*/


	return 0;
}
