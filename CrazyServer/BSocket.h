#pragma once
#include <process.h>

#include <WinSock2.h>
#include "../CrazyGameLib/gameProtocol.h"


#pragma comment(lib, "ws2_32")

#define READ       3
#define WRITE      5

#define ROOM_SIZE 10

typedef struct hanledata{

	SOCKET hClntSock;
	SOCKADDR_IN clntAdr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct iodata{

	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buf[BUF_SIZE];
	int rwMode;
} PER_IO_DATA, *LPPER_IO_DATA;

typedef struct room{

	int userCount;
	BOOL isPlay;

	int ready[MAX_USER_INROOM];
	SOCKET sock[MAX_USER_INROOM];
	BPlayer player[MAX_USER_INROOM];

	char map[MAP_HEIGHT][MAP_WIDTH];
} GAME_ROOM;

class BSocket
{
public:

	WSADATA wsaData;
	HANDLE hComPort;
	SYSTEM_INFO sysInfo;
	LPPER_IO_DATA ioInfo;
	LPPER_HANDLE_DATA handleInfo;
	
	SOCKET hServSock;
	SOCKADDR_IN servAdr;
	DWORD recvBytes, flags;

	S_HEAD head;
	int hLen;

	int serial;

public:
	BSocket(void);
	~BSocket(void);

	// func	
	void errorHandling(char* msg);
	void iniServer(int port);
	void recvData(SOCKET s, LPPER_IO_DATA ioInfo, DWORD flags);
	void requestHandling(SOCKET s, LPPER_IO_DATA ioInfo,  int flags);
	void setHeader(LPPER_IO_DATA ioInfo);
	int sendContact(SOCKET s, LPPER_IO_DATA ioInfo, int flags);
	int sendAwaite(SOCKET s, LPPER_IO_DATA ioInfo, int flags);
	int sendReady(SOCKET s, LPPER_IO_DATA ioInfo, int flags);
	int sendGameStart(SOCKET s, LPPER_IO_DATA ioInfo, int flags);
	int sendGameData(SOCKET s, LPPER_IO_DATA ioInfo, int flags);
	void initRoom(void);
	BPlayer* intoRoom(SOCKET s, S_AWAITE* pAwaite, BPlayer* pPlayer);
	LPPER_IO_DATA createIoInfo(int rwMode);
	void closeUser(SOCKET s);
	int sendGameDataRun(SOCKET s, LPPER_IO_DATA ioInfo, int flags);
};

